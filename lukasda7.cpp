#include <iostream>
#include <string>
#include <vector>

#include "camera.h"
#include "config.h"
#include "gameState.h"
#include "light.h"
#include "object.h"
#include "pgr.h"
#include "data/sceneGraph.h"

// ############################################################################
//                               OpenGL Stuff
// ############################################################################

/**
 * \brief Load and compile single shader program from the given shader paths.
 * \param vs Vertex shader path
 * \param fs Fragment shader path
 */
ShaderProgram loadShaderProgram(std::string vs, std::string fs) {
    GLuint shaders[] = {
        pgr::createShaderFromFile(GL_VERTEX_SHADER, SHADER_PATH + vs),
        pgr::createShaderFromFile(GL_FRAGMENT_SHADER, SHADER_PATH + fs),
        0
    };

    if (shaders[0] == 0)
        fprintf(stderr, "ERROR: Could not load shader: %s!\n", vs.c_str());
    else if (shaders[1] == 0)
        fprintf(stderr, "ERROR: Could not load shader: %s!\n", fs.c_str());

    ShaderProgram prog;
    prog.program = pgr::createProgram(shaders);

    // getting attribute locations
    prog.locations.position = glGetAttribLocation(prog.program, "position");
    prog.locations.normal = glGetAttribLocation(prog.program, "normal");
    prog.locations.texCoord = glGetAttribLocation(prog.program, "texCoord");

    // getting uniform locations
    prog.locations.projectionMatrix = glGetUniformLocation(prog.program, "projectionMatrix");
    prog.locations.viewMatrix = glGetUniformLocation(prog.program, "viewMatrix");
    prog.locations.modelMatrix = glGetUniformLocation(prog.program, "modelMatrix");
    prog.locations.normalMatrix = glGetUniformLocation(prog.program, "normalMatrix");

    prog.locations.diffuse = glGetUniformLocation(prog.program, "matDiffuse");
    prog.locations.specular = glGetUniformLocation(prog.program, "matSpecular");
    prog.locations.ambient = glGetUniformLocation(prog.program, "matAmbient");
    prog.locations.shininess = glGetUniformLocation(prog.program, "matShininess");

    prog.locations.fogCenter = glGetUniformLocation(prog.program, "fogCenter");
    prog.locations.fogColor = glGetUniformLocation(prog.program, "fogColor");
    prog.locations.fogRadius = glGetUniformLocation(prog.program, "fogRadius");
    prog.locations.fogDensity = glGetUniformLocation(prog.program, "fogDensity");

    prog.locations.elapsedTime = glGetUniformLocation(prog.program, "elapsedTime");
    prog.locations.ambientColor = glGetUniformLocation(prog.program, "ambientColor");
    prog.locations.cameraPosition = glGetUniformLocation(prog.program, "cameraPosition");
    prog.locations.asteroidLocation = glGetUniformLocation(prog.program, "asteroidLocation");
    prog.locations.hasDiffuseTexture = glGetUniformLocation(prog.program, "hasDiffuseTexture");
    prog.locations.mandelbrotAnimPaused = glGetUniformLocation(prog.program, "mandelbrotAnimPaused");
    prog.locations.mandelbrotAnimStarted = glGetUniformLocation(prog.program, "mandelbrotAnimStarted");
    prog.locations.mandelbrotAnimStartTime = glGetUniformLocation(prog.program, "mandelbrotAnimStartTime");
    prog.locations.mandelbrotAnimPauseTime = glGetUniformLocation(prog.program, "mandelbrotAnimPauseTime");

    prog.initialized = true;

    return prog;
}

/**
 * \brief Load and compile shader programs. Get attribute locations.
 */
void loadShaderPrograms() {
    // creating all shader programs
    mandelrotShaderProgram = loadShaderProgram("mandelbrot.vert", "mandelbrot.frag");
    phongShaderProgram = loadShaderProgram("phong.vert", "phong.frag");
    rocketFlameShaderProgram = loadShaderProgram("rocketFlame.vert", "rocketFlame.frag");
    skydomeShaderProgram = loadShaderProgram("skydome.vert", "skydome.frag");
    cloudsShaderProgram = loadShaderProgram("clouds.vert", "clouds.frag");

    // pushing them all into the global shader program vector
    shaderPrograms.push_back(mandelrotShaderProgram);
    shaderPrograms.push_back(phongShaderProgram);
    shaderPrograms.push_back(rocketFlameShaderProgram);
    shaderPrograms.push_back(skydomeShaderProgram);
    shaderPrograms.push_back(cloudsShaderProgram);
}

/**
 * \brief Delete all shader program objects.
 */
void cleanupShaderPrograms(void) {
    for (ShaderProgram shdPrg : shaderPrograms) {
        pgr::deleteProgramAndShaders(shdPrg.program);
    }
}

void setFogUniforms(const ShaderProgram& shaderProgram) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    if (shaderProgram.locations.fogCenter != -1)
        glUniform3fv(shaderProgram.locations.fogCenter, 1, glm::value_ptr(fogBall.center));

    if (shaderProgram.locations.fogColor != -1)
        glUniform3fv(shaderProgram.locations.fogColor, 1, glm::value_ptr(fogBall.color));

    if (shaderProgram.locations.fogRadius != -1)
        glUniform1f(shaderProgram.locations.fogRadius, fogBall.radius);

    if (shaderProgram.locations.fogDensity != -1)
        glUniform1f(shaderProgram.locations.fogDensity, fogBall.density);
}

void setLightUniforms(const ShaderProgram& shaderProgram,
                      const std::vector<Light*>& sceneLights) {
    if (!shaderProgram.initialized) return;

    const GLint lightCountLocation = glGetUniformLocation(shaderProgram.program,
                                                          "lightCount");

    const int uploadedLightCount = std::min(static_cast<int>(sceneLights.size()),
                                            MAX_SCENE_LIGHTS);

    if (lightCountLocation != -1) glUniform1i(lightCountLocation, uploadedLightCount);

    // getting and setting all light-related uniform arrays
    for (int i = 0; i < uploadedLightCount; ++i) {
        const Light* light = sceneLights[i];
        const std::string lightIndex = std::to_string(i);
        const glm::vec3 lightPosition = glm::vec3(light->getGlobalModelMatrix()[3]);
        const glm::vec3 lightAmbient = light->getAmbient();
        const glm::vec3 lightDiffuse = light->getDiffuse();
        const glm::vec3 lightSpecular = light->getSpecular();
        const glm::vec3 lightSpotDirection = glm::normalize(
                                                glm::mat3(light->getGlobalModelMatrix())
                                                * light->getSpotDirection());
        const float lightSpotCutOff = light->getSpotCutOff();
        const float lightSpotExponent = light->getSpotExponent();
        const float lightIntensity = light->getIntensity();
        const int lightType = static_cast<int>(light->getLightType());

        const GLint typeLocation = glGetUniformLocation(
            shaderProgram.program, ("lightTypes[" + lightIndex + "]").c_str());

        const GLint ambientLocation = glGetUniformLocation(
            shaderProgram.program, ("lightAmbients[" + lightIndex + "]").c_str());

        const GLint diffuseLocation = glGetUniformLocation(
            shaderProgram.program, ("lightDiffuses[" + lightIndex + "]").c_str());

        const GLint specularLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpeculars[" + lightIndex + "]").c_str());

        const GLint positionLocation = glGetUniformLocation(
            shaderProgram.program, ("lightPositions[" + lightIndex + "]").c_str());

        const GLint spotDirectionLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpotDirections[" + lightIndex + "]").c_str());

        const GLint spotCutOffLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpotCutOffs[" + lightIndex + "]").c_str());

        const GLint spotExponentLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpotExponents[" + lightIndex + "]").c_str());

        const GLint intensityLocation = glGetUniformLocation(
            shaderProgram.program, ("lightIntensities[" + lightIndex + "]").c_str());

        if (typeLocation != -1)
            glUniform1i(typeLocation, lightType);

        if (ambientLocation != -1)
            glUniform3fv(ambientLocation, 1, glm::value_ptr(lightAmbient));

        if (diffuseLocation != -1)
            glUniform3fv(diffuseLocation, 1, glm::value_ptr(lightDiffuse));

        if (specularLocation != -1)
            glUniform3fv(specularLocation, 1, glm::value_ptr(lightSpecular));

        if (positionLocation != -1)
            glUniform3fv(positionLocation, 1, glm::value_ptr(lightPosition));

        if (spotDirectionLocation != -1)
            glUniform3fv(spotDirectionLocation, 1, glm::value_ptr(lightSpotDirection));

        if (spotCutOffLocation != -1)
            glUniform1f(spotCutOffLocation, lightSpotCutOff);

        if (spotExponentLocation != -1)
            glUniform1f(spotExponentLocation, lightSpotExponent);

        if (intensityLocation != -1)
            glUniform1f(intensityLocation, lightIntensity);
    }
}

/**
 * \brief Draw all scene objects and update their uniforms
 */
void drawScene(void) {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix =
        camera.getProjectionMatrix((float)gameState.windowWidth /
                                   (float)gameState.windowHeight);
    const glm::vec3 cameraPosition = camera.getPosition();

    glUseProgram(phongShaderProgram.program);

    // elapsed time uniform update
    if (phongShaderProgram.locations.elapsedTime != -1)
        glUniform1f(phongShaderProgram.locations.elapsedTime, gameState.elapsedTime);

    // camera position uniform update
    if (phongShaderProgram.locations.cameraPosition != -1) {
        glUniform3fv(phongShaderProgram.locations.cameraPosition,
                     1, glm::value_ptr(cameraPosition));
    }

    // ambient uniform update
    if (phongShaderProgram.locations.ambientColor != -1)
        glUniform3fv(phongShaderProgram.locations.ambientColor,
                     1, glm::value_ptr(gameState.ambientColor));

    // light uniforms update
    setLightUniforms(phongShaderProgram, sceneLightsCache);

    glUseProgram(cloudsShaderProgram.program);

    // elapsed time uniform update
    if (cloudsShaderProgram.locations.elapsedTime != -1)
        glUniform1f(cloudsShaderProgram.locations.elapsedTime, gameState.elapsedTime);

    // camera position uniform update
    if (cloudsShaderProgram.locations.cameraPosition != -1) {
        glUniform3fv(cloudsShaderProgram.locations.cameraPosition,
                     1, glm::value_ptr(cameraPosition));
    }

    // ambient uniform update
    if (cloudsShaderProgram.locations.ambientColor != -1)
        glUniform3fv(cloudsShaderProgram.locations.ambientColor,
                     1, glm::value_ptr(gameState.ambientColor));

    // light uniforms update
    setLightUniforms(cloudsShaderProgram, sceneLightsCache);

    // fog uniforms update for all shaders
    for (const ShaderProgram& prog : shaderPrograms)
        setFogUniforms(prog);

    glUseProgram(skydomeShaderProgram.program);
    if (skydomeShaderProgram.locations.cameraPosition != -1)
        glUniform3fv(skydomeShaderProgram.locations.cameraPosition,
                     1, glm::value_ptr(cameraPosition));

    // update time and animation uniforms in the animated Mandelbrot shader
    glUseProgram(mandelrotShaderProgram.program);
    glUniform1f(mandelrotShaderProgram.locations.elapsedTime, gameState.elapsedTime);
    glUniform1f(mandelrotShaderProgram.locations.mandelbrotAnimStartTime,
                gameState.mandelbrotAnimStartTime);
    glUniform1i(mandelrotShaderProgram.locations.mandelbrotAnimStarted,
                gameState.mandelbrotAnimStarted);
    glUniform1i(mandelrotShaderProgram.locations.mandelbrotAnimPaused,
                gameState.mandelbrotAnimPaused);
    glUniform1f(mandelrotShaderProgram.locations.mandelbrotAnimPauseTime,
                gameState.mandelbrotAnimPauseTime);

    // update time in the animated rocket flame shader
    glUseProgram(rocketFlameShaderProgram.program);
    glUniform3fv(rocketFlameShaderProgram.locations.cameraPosition,
                 1, glm::value_ptr(cameraPosition));
    glUniform1f(rocketFlameShaderProgram.locations.elapsedTime, gameState.elapsedTime);

    // draw all scene objects recursively from the root node
    sceneRoot.draw(viewMatrix, projectionMatrix);
}

void updateCamera(float deltaTime) {
    // toggling camera sprint
    camera.setSprinting(gameState.keyMap[KEY_SHIFT]);

    // camera movement
    camera.moveForward((gameState.keyMap[KEY_W]) * deltaTime);
    camera.moveBackward((gameState.keyMap[KEY_S]) * deltaTime);
    camera.moveRight((gameState.keyMap[KEY_D]) * deltaTime);
    camera.moveLeft((gameState.keyMap[KEY_A]) * deltaTime);
    camera.moveUp((gameState.keyMap[KEY_E]) * deltaTime);
    camera.moveDown((gameState.keyMap[KEY_Q]) * deltaTime);

    // camera rotation is handled directly in mouseMotionCb() callback

    // camera FOV change
    // branchless way of saying if DOWN then decrease else if UP then increase
    camera.changeFieldOfView((gameState.keyMap[KEY_ARROW_DOWN]) * deltaTime -
                             (gameState.keyMap[KEY_ARROW_UP]) * deltaTime);
}

void rotatePlanetarySystem(float deltaTime) {
    // -----------------------  moon rotation   -----------------------
    static float moonOrbitAngle = 0.0f;
    static float moonAxisAngle = 0.0f;

    // subtracting planet's rotation speed since moon is its child
    moonOrbitAngle += glm::radians(deltaTime *
                                   (MOON_ORBIT_ROT_SPEED - PLANET_1_AXIS_ROT_SPEED) *
                                   GLOBAL_ANIM_SPEED);
    moonAxisAngle += glm::radians(deltaTime *
                                  MOON_AXIS_ROT_SPEED * 
                                  GLOBAL_ANIM_SPEED);

    // rotate moon around the planet
    glm::mat4 moonOrbit = glm::rotate(glm::mat4(1.0f),
                                      moonOrbitAngle,
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    // moon's center's offset relative to the planet center
    glm::mat4 moonOffset = glm::translate(glm::mat4(1.0f),
                                          MOON_POSITION);

    // rotate moon around its own axis
    glm::mat4 moonSelfRotation = glm::rotate(glm::mat4(1.0f),
                                             moonAxisAngle,
                                             glm::vec3(0.0f, 1.0f, 0.0f));

    moon->setLocalModelMatrix(moonOrbit * moonOffset * moonSelfRotation);

    // -----------------------  ufo rotation   ------------------------

    static float ufoOrbitAngle = 0.0f;
    static float ufoAxisAngle = 0.0f;

    // subtracting moon's rotation speed since ufo is its child
    ufoOrbitAngle += glm::radians(deltaTime * UFO_ORBIT_ROT_SPEED * GLOBAL_ANIM_SPEED);
    ufoAxisAngle += glm::radians(deltaTime * UFO_AXIS_ROT_SPEED * GLOBAL_ANIM_SPEED);

    // rotate ufo around the moon
    glm::mat4 ufoOrbit = glm::rotate(glm::mat4(1.0f),
                                     ufoOrbitAngle,
                                     glm::vec3(1.0f, 0.0f, 0.0f));

    // ufo's center's offset relative to the moon center
    glm::mat4 ufoOffset = glm::translate(glm::mat4(1.0f),
                                         UFO_POSITION);

    // rotate ufo around its own axis
    glm::mat4 ufoSelfRotation = glm::rotate(glm::mat4(1.0f),
                                            ufoAxisAngle,
                                            glm::vec3(0.0f, 1.0f, 0.0f));

    ufo->setLocalModelMatrix(glm::inverse(moonSelfRotation) * ufoOrbit *
                             ufoOffset * ufoSelfRotation);

    // -------------------  shared planets orbit  ---------------------
    static float planetsOrbitAngle = 0.0f;

    planetsOrbitAngle += glm::radians(deltaTime * PLANET_ORBIT_ROT_SPEED * GLOBAL_ANIM_SPEED);

    glm::mat4 planetsOrbit = glm::translate(glm::mat4(1.0f), 
                                            glm::vec3(0.0f, 80.0f, 0.0f)) *
                             glm::rotate(glm::mat4(1.0f),
                                         planetsOrbitAngle,
                                         glm::vec3(0.0f, 1.0f, 0.0f)) *
                             glm::translate(glm::mat4(1.0f),
                                            glm::vec3(0.0f, -80.0f, 0.0f));

    // ---------------------  planet 1 rotation   ---------------------
    static float planet1AxisAngle = 0.0f;

    planet1AxisAngle += glm::radians(deltaTime *
                                     (PLANET_1_AXIS_ROT_SPEED - PLANET_ORBIT_ROT_SPEED) * 
                                     GLOBAL_ANIM_SPEED);

    glm::mat4 planet1Offset = glm::translate(glm::mat4(1.0f), PLANET_1_POSITION);
    glm::mat4 planet1SelfRotation = glm::rotate(glm::mat4(1.0f),
                                                planet1AxisAngle,
                                                glm::vec3(0.0f, 1.0f, 0.0f));

    planet1->setLocalModelMatrix(planetsOrbit * planet1Offset * planet1SelfRotation);

    // ---------------------  planet 2 rotation   ---------------------
    static float planet2AxisAngle = 0.0f;

    planet2AxisAngle += glm::radians(deltaTime *
                                     (PLANET_2_AXIS_ROT_SPEED - PLANET_ORBIT_ROT_SPEED) * 
                                     GLOBAL_ANIM_SPEED);

    glm::mat4 planet2Offset = glm::translate(glm::mat4(1.0f), PLANET_2_POSITION);
    glm::mat4 planet2SelfRotation = glm::rotate(glm::mat4(1.0f),
                                                planet2AxisAngle,
                                                glm::vec3(0.0f, 1.0f, 0.0f));

    planet2->setLocalModelMatrix(planetsOrbit * planet2Offset * planet2SelfRotation);
}

// ############################################################################
//                                Callbacks
// ############################################################################

/**
 * \brief Draw the window contents.
 */
void displayCb() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the window contents (scene objects)
    drawScene();

    glutSwapBuffers();
}

/**
 * \brief Window was reshaped.
 * \param newWidth New window width
 * \param newHeight New window height
 */
void reshapeCb(int newWidth, int newHeight) {
    gameState.windowWidth = newWidth;
    gameState.windowHeight = newHeight;

    glViewport(0, 0, newWidth, newHeight);
}

/**
 * \brief Handle the key pressed event.
 * Called whenever a key on the keyboard was pressed. The key is given by the "keyPressed"
 * parameter, which is an ASCII character. It's often a good idea to have the escape key (ASCII value 27)
 * to call glutLeaveMainLoop() to exit the program.
 * \param keyPressed ASCII code of the key
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void keyboardCb(unsigned char keyPressed, int mouseX, int mouseY) {
    switch (keyPressed) {
        // exit
        case 27:
            glutLeaveMainLoop();
            exit(EXIT_SUCCESS);
            break;
        // movement cases
        case 'w': case 'W': gameState.keyMap[KEY_W] = true; break;
        case 's': case 'S': gameState.keyMap[KEY_S] = true; break;
        case 'a': case 'A': gameState.keyMap[KEY_A] = true; break;
        case 'd': case 'D': gameState.keyMap[KEY_D] = true; break;
        case 'q': case 'Q': gameState.keyMap[KEY_Q] = true; break;
        case 'e': case 'E': gameState.keyMap[KEY_E] = true; break;

        // Mandelbrot animation toggle (start/pause/resume)
        case 'p': case 'P': {
            if (!gameState.mandelbrotAnimStarted) {
                gameState.mandelbrotAnimStarted = true;
                gameState.mandelbrotAnimPaused = false;
                gameState.mandelbrotAnimStartTime = gameState.elapsedTime;
                gameState.mandelbrotAnimPauseTime = gameState.elapsedTime;
            } else if (!gameState.mandelbrotAnimPaused) {
                gameState.mandelbrotAnimPaused = true;
                gameState.mandelbrotAnimPauseTime = gameState.elapsedTime;
            } else {
                gameState.mandelbrotAnimPaused = false;
                gameState.mandelbrotAnimStartTime +=
                    (gameState.elapsedTime - gameState.mandelbrotAnimPauseTime);
            }
            break;
        }
    }
}

// Called whenever a key on the keyboard was released. The key
// is given by the "keyReleased" parameter, which is in ASCII.
/**
 * \brief Handle the key released event.
 * \param keyReleased ASCII code of the released key
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY) {
    switch (keyReleased) {
        case 'w': case 'W': gameState.keyMap[KEY_W] = false; break;
        case 's': case 'S': gameState.keyMap[KEY_S] = false; break;
        case 'a': case 'A': gameState.keyMap[KEY_A] = false; break;
        case 'd': case 'D': gameState.keyMap[KEY_D] = false; break;
        case 'q': case 'Q': gameState.keyMap[KEY_Q] = false; break;
        case 'e': case 'E': gameState.keyMap[KEY_E] = false; break;
    }
}

//
/**
 * \brief Handle the non-ASCII key pressed event (such as arrows or F1).
 *  The special keyboard callback is triggered when keyboard function (Fx) or directional
 *  keys are pressed.
 * \param specKeyPressed int value of a predefined glut constant such as GLUT_KEY_UP
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void specialKeyboardCb(int specKeyPressed, int mouseX, int mouseY) {
    switch (specKeyPressed) {
        case GLUT_KEY_UP:   gameState.keyMap[KEY_ARROW_UP]   = true; break;
        case GLUT_KEY_DOWN: gameState.keyMap[KEY_ARROW_DOWN] = true; break;
    }
    
    gameState.keyMap[KEY_SHIFT] = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
}

void specialKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY) {
    switch (specKeyReleased) {
        case GLUT_KEY_UP:   gameState.keyMap[KEY_ARROW_UP]   = false; break;
        case GLUT_KEY_DOWN: gameState.keyMap[KEY_ARROW_DOWN] = false; break;
    }

    gameState.keyMap[KEY_SHIFT] = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
}

// -----------------------  Mouse ---------------------------------
// three events - mouse click, mouse drag, and mouse move with no button pressed

// 
/**
 * \brief React to mouse button press and release (mouse click).
 * When the user presses and releases mouse buttons in the window, each press
 * and each release generates a mouse callback (including release after dragging).
 *
 * \param buttonPressed button code (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON)
 * \param buttonState GLUT_DOWN when pressed, GLUT_UP when released
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void mouseCb(int buttonPressed, int buttonState, int mouseX, int mouseY) {
    switch (buttonPressed) {
        case GLUT_LEFT_BUTTON:
            gameState.keyMap[LMB] = (buttonState == GLUT_DOWN);
            break;
        case GLUT_MIDDLE_BUTTON:
            gameState.keyMap[MMB] = (buttonState == GLUT_DOWN);
            break;
        case GLUT_RIGHT_BUTTON:
            gameState.keyMap[RMB] = (buttonState == GLUT_DOWN);
            break;
    }
    
    gameState.ogMouseX = mouseX;
    gameState.ogMouseY = mouseY;
}

/**
 * \brief Handle mouse dragging (mouse move with any button pressed).
 *        This event follows the glutMouseFunc(mouseCb) event.
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void mouseMotionCb(int mouseX, int mouseY) {
    if (gameState.keyMap[RMB]) {
        int deltaX = (gameState.ogMouseX - mouseX);
        int deltaY = (gameState.ogMouseY - mouseY);

        camera.rotate(-deltaX * gameState.mouseSensitivity,
                       deltaY * gameState.mouseSensitivity);

        gameState.ogMouseX = mouseX;
        gameState.ogMouseY = mouseY;
    }
}

/**
 * \brief Handle mouse movement over the window (with no button pressed).
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void passiveMouseMotionCb(int mouseX, int mouseY) {

    // mouse hovering over window

    // create display event to redraw window contents if needed (and not handled in the timer callback)
    // glutPostRedisplay();
}

/**
 * \brief Callback responsible for the scene update.
 */
void timerCb(int) {
    // getting current time and time from last frame
    static float lastTime = 0.0f;
    gameState.elapsedTime = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME)); // ms
    float deltaTime = gameState.elapsedTime - lastTime;
    lastTime = gameState.elapsedTime;

    // update the application state
    sceneRoot.update(gameState.elapsedTime, nullptr);
    updateCamera(deltaTime);
    rotatePlanetarySystem(deltaTime);

    // update the fog's position to follow planet1
    fogBall.center = planet1->getPosition();
    planet2Clouds->setPosition(planet2->getPosition());
    
    // and plan a new event
    glutTimerFunc(33, timerCb, 0);

    // create display event
    glutPostRedisplay();
}

// ############################################################################
//                                 Application
// ############################################################################

/**
 * \brief Initialize application data and OpenGL stuff.
 */
void initApplication() {
    // init OpenGL
    // - all programs (shaders), buffers, textures, ...
    loadShaderPrograms();

    // enable services like depth test, backface culling, blending etc.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    // create objects and add them to the scene graph
    createObjects();

    // create lights and a light-cache storing lights only for fast uniforms
    createLights();

    // initialize global transforms once
    sceneRoot.update(0.0f, nullptr);
}

/**
 * \brief Delete all OpenGL objects and application data.
 */
void finalizeApplication(void) {

    // cleanUpObjects();

    // delete buffers
    // cleanupModels();

    // delete shaders
    cleanupShaderPrograms();
}

// ############################################################################
//                                   Main
// ############################################################################


/**
 * \brief Entry point of the application.
 * \param argc number of command line arguments
 * \param argv array with argument strings
 * \return 0 if OK, <> elsewhere
 */
int main(int argc, char** argv) {

    // initialize the GLUT library (windowing system)
    glutInit(&argc, argv);

    glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    // multisampling level
    glutSetOption(GLUT_MULTISAMPLE, 4);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    // for each window
    {
        // initial window size + title
        glutInitWindowSize(gameState.windowWidth, gameState.windowHeight);
        glutCreateWindow(WINDOW_TITLE);

        // callbacks - use only those you need
        glutDisplayFunc(displayCb);
        glutReshapeFunc(reshapeCb);
        glutKeyboardFunc(keyboardCb);
        glutKeyboardUpFunc(keyboardUpCb);
        glutSpecialFunc(specialKeyboardCb);
        glutSpecialUpFunc(specialKeyboardUpCb);
        glutMouseFunc(mouseCb);
        glutMotionFunc(mouseMotionCb);

        glutTimerFunc(33, timerCb, 0);

    }
    // end for each window 

    // initialize pgr-framework (GL, DevIl, etc.)
    if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
        pgr::dieWithError("pgr init failed, required OpenGL not supported?");

    // init your stuff - shaders & program, buffers, locations, state of the application
    initApplication();

    // handle window close by the user
    glutCloseFunc(finalizeApplication);

    // Infinite loop handling the events
    glutMainLoop();

    // code after glutLeaveMainLoop()
    // cleanup

    return EXIT_SUCCESS;
}
