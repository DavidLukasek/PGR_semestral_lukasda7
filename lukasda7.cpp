//----------------------------------------------------------------------------------------
/**
 * \file       lukasda7.cpp
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Application entry point.
 *
 *  Contains main initialization, render loop orchestration and high-level application wiring.
 *
*/
//----------------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>

#include "camera.h"
#include "gameState.h"
#include "light.h"
#include "object.h"
#include "pgr.h"
#include "utils.h"
#include "data/sceneGraph.h"

// ############################################################################
//                               OpenGL Stuff
// ############################################################################

/**
 * \brief      Application entry point.
 * \param vs Vertex shader path.
 * \param fs Fragment shader path.
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
    prog.locations.pvmMatrix = glGetUniformLocation(prog.program, "pvmMatrix");

    prog.locations.diffuse = glGetUniformLocation(prog.program, "matDiffuse");
    prog.locations.specular = glGetUniformLocation(prog.program, "matSpecular");
    prog.locations.ambient = glGetUniformLocation(prog.program, "matAmbient");
    prog.locations.shininess = glGetUniformLocation(prog.program, "matShininess");

    prog.locations.fogCenter = glGetUniformLocation(prog.program, "fogCenter");
    prog.locations.fogCenter2 = glGetUniformLocation(prog.program, "fogCenter2");
    prog.locations.fogColor = glGetUniformLocation(prog.program, "fogColor");
    prog.locations.fogColor2 = glGetUniformLocation(prog.program, "fogColor2");
    prog.locations.fogRadius = glGetUniformLocation(prog.program, "fogRadius");
    prog.locations.fogRadius2 = glGetUniformLocation(prog.program, "fogRadius2");
    prog.locations.fogDensity = glGetUniformLocation(prog.program, "fogDensity");
    prog.locations.fogDensity2 = glGetUniformLocation(prog.program, "fogDensity2");

    prog.locations.elapsedTime = glGetUniformLocation(prog.program, "elapsedTime");
    prog.locations.ambientColor = glGetUniformLocation(prog.program, "ambientColor");
    prog.locations.cameraPosition = glGetUniformLocation(prog.program, "cameraPosition");
    prog.locations.asteroidLocation = glGetUniformLocation(prog.program, "asteroidLocation");
    prog.locations.hasDiffuseTexture = glGetUniformLocation(prog.program, "hasDiffuseTexture");
    prog.locations.hasNormalTexture = glGetUniformLocation(prog.program, "hasNormalTexture");
    prog.locations.hasSpecularTexture = glGetUniformLocation(prog.program, "hasSpecularTexture");
    prog.locations.isUVAnimated = glGetUniformLocation(prog.program, "isUVAnimated");
    prog.locations.isDisplaceAnimated = glGetUniformLocation(prog.program, "isDisplaceAnimated");
    prog.locations.displacementSize = glGetUniformLocation(prog.program, "displacementSize");

    prog.locations.mandelbrotAnimPaused = glGetUniformLocation(prog.program, "mandelbrotAnimPaused");
    prog.locations.mandelbrotAnimStarted = glGetUniformLocation(prog.program, "mandelbrotAnimStarted");
    prog.locations.mandelbrotAnimStartTime = glGetUniformLocation(prog.program, "mandelbrotAnimStartTime");
    prog.locations.mandelbrotAnimPauseTime = glGetUniformLocation(prog.program, "mandelbrotAnimPauseTime");
    prog.locations.mandelbrotMaxIterations = glGetUniformLocation(prog.program, "mandelbrotMaxIterations");
    prog.locations.mandelbrotZoomSpeed = glGetUniformLocation(prog.program, "mandelbrotZoomSpeed");
    prog.locations.mandelbrotColorSpeed = glGetUniformLocation(prog.program, "mandelbrotColorSpeed");
    prog.locations.mandelbrotZoomTarget = glGetUniformLocation(prog.program, "mandelbrotZoomtarget");

    prog.initialized = true;

    return prog;
}

/**
 * \brief      Application entry point.
 */
void loadShaderPrograms() {
    // creating all shader programs
    mandelrotShaderProgram = loadShaderProgram("mandelbrot.vert", "mandelbrot.frag");
    phongShaderProgram = loadShaderProgram("phong.vert", "phong.frag");
    rocketFlameShaderProgram = loadShaderProgram("rocketFlame.vert", "rocketFlame.frag");
    skydomeShaderProgram = loadShaderProgram("skydome.vert", "skydome.frag");
    displacementShaderProgram = loadShaderProgram("displacement.vert", "phong.frag");

    // pushing them all into the global shader program vector
    shaderPrograms.push_back(mandelrotShaderProgram);
    shaderPrograms.push_back(phongShaderProgram);
    shaderPrograms.push_back(rocketFlameShaderProgram);
    shaderPrograms.push_back(skydomeShaderProgram);
    shaderPrograms.push_back(displacementShaderProgram);
}

/**
 * \brief      Application entry point.
 */
void cleanupShaderPrograms(void) {
    for (ShaderProgram shdPrg : shaderPrograms) {
        pgr::deleteProgramAndShaders(shdPrg.program);
    }
}

/**
 * \brief      Application entry point.
 */
void drawScene(void) {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix =
        camera.getProjectionMatrix((float)gameState.windowWidth /
                                   (float)gameState.windowHeight);
    const glm::vec3 cameraPosition = camera.getPosition();

    // elapsed time, camera position and ambient color uniform update
    setMiscUniforms(phongShaderProgram, gameState.elapsedTime,
                     cameraPosition, gameState.ambientColor);
    setMiscUniforms(displacementShaderProgram, gameState.elapsedTime,
                     cameraPosition, gameState.ambientColor);

    // environment map uniform update
    setEnvironmentMapUniforms(phongShaderProgram, environmentMapTextureObject);

    // light uniforms update
    setLightUniforms(phongShaderProgram, sceneLightsCache);
    setLightUniforms(displacementShaderProgram, sceneLightsCache);

    // fog uniforms update for all shaders
    for (const ShaderProgram& shaderProgram : shaderPrograms)
        setFogUniforms(shaderProgram,
                       fogBall1.center,
                       fogBall2.center,
                       fogBall1.color,
                       fogBall2.color,
                       fogBall1.radius,
                       fogBall2.radius,
                       fogBall1.density,
                       fogBall2.density);

    setSkydomeUniforms(skydomeShaderProgram, cameraPosition);

    // update time and animation uniforms in the animated Mandelbrot shader
    setMandelbrotAnimationUniforms(mandelrotShaderProgram,
                                   gameState.elapsedTime,
                                   gameState.mandelbrotAnimStartTime,
                                   gameState.mandelbrotAnimStarted,
                                   gameState.mandelbrotAnimPaused,
                                   gameState.mandelbrotAnimPauseTime);

    // update time in the animated rocket flame shader
    setRocketFlameUniforms(rocketFlameShaderProgram,
                           cameraPosition,
                           gameState.elapsedTime);

    // draw all scene objects recursively from the root node
    sceneRoot.draw(viewMatrix, projectionMatrix);
}

// ############################################################################
//                                Callbacks
// ############################################################################

/**
 * \brief      Application entry point.
 */
void displayCb() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // draw the window contents (scene objects)
    drawScene();

    glutSwapBuffers();
}

/**
 * \brief      Application entry point.
 * \param newWidth New window width.
 * \param newHeight New window height.
 */
void reshapeCb(int newWidth, int newHeight) {
    gameState.windowWidth = newWidth;
    gameState.windowHeight = newHeight;

    glViewport(0, 0, newWidth, newHeight);
}

/**
 * \brief      Application entry point.
 * Called whenever a key on the keyboard was pressed. The key is given by the "keyPressed"
 * parameter, which is an ASCII character. It's often a good idea to have the escape key (ASCII value 27)
 * to call glutLeaveMainLoop() to exit the program.
 * \param keyPressed ASCII code of the key.
 * \param mouseX Mouse (cursor) X position.
 * \param mouseY Mouse (cursor) Y position.
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
        case 'c': case 'C':
            saveCurrentCameraToPreset(camera,
                                      cameraPresets[gameState.currentCameraPresetIndex].position,
                                      cameraPresets[gameState.currentCameraPresetIndex].rotation);
            gameState.currentCameraPresetIndex = (gameState.currentCameraPresetIndex + 1) % 3;
            applyCameraPreset(camera,
                              cameraPresets[gameState.currentCameraPresetIndex].position,
                              cameraPresets[gameState.currentCameraPresetIndex].rotation);
            break;
    }
}

// Called whenever a key on the keyboard was released. The key
// is given by the "keyReleased" parameter, which is in ASCII.
/**
 * \brief      Application entry point.
 * \param keyReleased ASCII code of the released key.
 * \param mouseX Mouse (cursor) X position.
 * \param mouseY Mouse (cursor) Y position.
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
 * \brief      Application entry point.
 *  The special keyboard callback is triggered when keyboard function (Fx) or directional
 *  keys are pressed.
 * \param specKeyPressed Int value of a predefined glut constant such as GLUT_KEY_UP.
 * \param mouseX Mouse (cursor) X position.
 * \param mouseY Mouse (cursor) Y position.
 */
void specialKeyboardCb(int specKeyPressed, int mouseX, int mouseY) {
    switch (specKeyPressed) {
        case GLUT_KEY_UP:   gameState.keyMap[KEY_ARROW_UP]   = true; break;
        case GLUT_KEY_DOWN: gameState.keyMap[KEY_ARROW_DOWN] = true; break;
    }
    
    gameState.keyMap[KEY_SHIFT] = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
}

/**
 * \brief      Application entry point.
 * \param specKeyReleased Int value of a predefined glut constant such as GLUT_KEY_UP.
 * \param mouseX Mouse (cursor) X position.
 * \param mouseY Mouse (cursor) Y position.
 */
void specialKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY) {
    switch (specKeyReleased) {
        case GLUT_KEY_UP:   gameState.keyMap[KEY_ARROW_UP]   = false; break;
        case GLUT_KEY_DOWN: gameState.keyMap[KEY_ARROW_DOWN] = false; break;
    }

    gameState.keyMap[KEY_SHIFT] = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
}

// 
/**
 * \brief      Application entry point.
 * When the user presses and releases mouse buttons in the window, each press
 * and each release generates a mouse callback (including release after dragging).
 *
 * \param buttonPressed Button code (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON).
 * \param buttonState GLUT_DOWN when pressed, GLUT_UP when released.
 * \param mouseX Mouse (cursor) X position.
 * \param mouseY Mouse (cursor) Y position.
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
 * \brief      Application entry point.
 *        This event follows the glutMouseFunc(mouseCb) event.
 * \param mouseX Mouse (cursor) X position.
 * \param mouseY Mouse (cursor) Y position.
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
 * \brief      Application entry point.
 * \param mouseX Mouse (cursor) X position.
 * \param mouseY Mouse (cursor) Y position.
 */
void passiveMouseMotionCb(int mouseX, int mouseY) {

    // mouse hovering over window

    // create display event to redraw window contents if needed (and not handled in the timer callback)
    // glutPostRedisplay();
}

/**
 * \brief      Application entry point.
 */
void timerCb(int) {
    // getting current time and time from last frame
    static float lastTime = 0.0f;
    gameState.elapsedTime = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME)); // ms
    float deltaTime = gameState.elapsedTime - lastTime;
    lastTime = gameState.elapsedTime;

    CameraInputState cameraInput = {};
    cameraInput.moveForward = gameState.keyMap[KEY_W];
    cameraInput.moveBackward = gameState.keyMap[KEY_S];
    cameraInput.moveRight = gameState.keyMap[KEY_D];
    cameraInput.moveLeft = gameState.keyMap[KEY_A];
    cameraInput.moveUp = gameState.keyMap[KEY_E];
    cameraInput.moveDown = gameState.keyMap[KEY_Q];
    cameraInput.fovUpPressed = gameState.keyMap[KEY_ARROW_UP];
    cameraInput.fovDownPressed = gameState.keyMap[KEY_ARROW_DOWN];
    cameraInput.sprint = gameState.keyMap[KEY_SHIFT];

    // update the application state
    updateCamera(deltaTime,
                 camera,
                 cameraInput,
                 glm::vec3(0.0f),
                 BOUNDING_SPHERE_RADIUS,
                 planet1,
                 102.5f,
                 planet2,
                 102.5f,
                 ufo,
                 cameraPresets[2].position,
                 cameraPresets[2].rotation,
                 gameState.currentCameraPresetIndex);
    rotatePlanetarySystem(deltaTime, moon, ufo, planet1, planet2);
    updateStageLightsAnimation(deltaTime,
                               gameState.stageLightsAnimRunning,
                               stageLightsCache,
                               lightHoldersCache);
    sceneRoot.update(gameState.elapsedTime, nullptr);

    // update the fog's positions to follow planets
    fogBall1.center = planet1->getPosition();
    fogBall2.center = planet2->getPosition();
    planet2Clouds->setPosition(planet2->getPosition());

    const glm::mat4 viewMatrix = camera.getViewMatrix();
    const glm::mat4 projectionMatrix =
        camera.getProjectionMatrix((float)gameState.windowWidth /
                                   (float)gameState.windowHeight);

    // check if an object has been picked and which
    checkObjectPick(gameState.keyMap[LMB],
                    gameState.ogMouseX,
                    gameState.ogMouseY,
                    gameState.windowHeight,
                    &sceneRoot,
                    viewMatrix,
                    projectionMatrix,
                    gameState.mandelbrotAnimStarted,
                    gameState.mandelbrotAnimPaused,
                    gameState.mandelbrotAnimStartTime,
                    gameState.mandelbrotAnimPauseTime,
                    gameState.elapsedTime,
                    gameState.stageLightsAnimStarted,
                    gameState.stageLightsAnimRunning,
                    stageLightsCache,
                    lightHoldersCache,
                    gameState.rocketFlamesEnabled,
                    planet1,
                    rocketFlame1,
                    rocketFlame2);

    // and plan a new event
    glutTimerFunc(33, timerCb, 0);

    // create display event
    glutPostRedisplay();
}

// ############################################################################
//                                 Application
// ############################################################################

/**
 * \brief      Application entry point.
 */
void initApplication() {
    applyCameraPreset(camera,
                      cameraPresets[gameState.currentCameraPresetIndex].position,
                      cameraPresets[gameState.currentCameraPresetIndex].rotation);

    // init OpenGL
    // - all programs (shaders), buffers, textures, ...
    loadShaderPrograms();
    if (displacementShaderProgram.initialized) {
        glUseProgram(displacementShaderProgram.program);
        if (displacementShaderProgram.locations.displacementSize != -1)
            glUniform1f(displacementShaderProgram.locations.displacementSize,
                        PLANET_1_DISPLACEMENT_SIZE);
    }
    setMandelbrotStaticUniforms(mandelrotShaderProgram);
    environmentMapTextureObject = pgr::createTexture("data/textures/space_skydome_black_hole.png");
    if (environmentMapTextureObject == 0) {
        std::cerr << "ERROR: Could not load environment texture: data/textures/space_skydome_black_hole.png"
                  << std::endl;
    }

    // enable services like depth test, backface culling, blending etc.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);

    // set up alpha-blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create objects and add them to the scene graph
    createObjects();

    // create lights and a light-cache storing lights only for fast uniforms
    createLights();

    // initialize global transforms once
    sceneRoot.update(0.0f, nullptr);
}

/**
 * \brief      Application entry point.
 */
void finalizeApplication(void) {

    // cleanUpObjects();

    // delete buffers
    // cleanupModels();

    // delete shaders
    cleanupShaderPrograms();

    if (environmentMapTextureObject != 0) {
        glDeleteTextures(1, &environmentMapTextureObject);
        environmentMapTextureObject = 0;
    }
}

// ############################################################################
//                                   Main
// ############################################################################


/**
 * \brief      Application entry point.
 * \param argc Number of command line arguments.
 * \param argv Array with argument strings.
 * \return 0 if OK, <> elsewhere.
 */
int main(int argc, char** argv) {

    // initialize the GLUT library (windowing system)
    glutInit(&argc, argv);

    glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    // multisampling level
    glutSetOption(GLUT_MULTISAMPLE, 4);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);

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
