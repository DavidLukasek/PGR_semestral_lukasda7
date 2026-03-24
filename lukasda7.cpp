#include <iostream>

#include "camera.h"
#include "data/models/square.h"
#include "gameState.h"
#include "light.h"
#include "object.h"
#include "pgr.h"
#include "singlemesh.h"

ObjectList objects;
ShaderProgram commonShaderProgram;
ShaderProgram mandelrotShaderProgram;

Camera camera;
glm::vec2 cameraRotation = glm::vec2(0.0);

int ogMouseX = -1;
int ogMouseY = -1;
const float mouseSensitivity = 0.3f;

GameState gameState;

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

    prog.locations.position = glGetAttribLocation(prog.program, "position");
    prog.locations.texCoord = glGetAttribLocation(prog.program, "texCoord");

    prog.locations.PVMmatrix = glGetUniformLocation(prog.program, "PVM");
    prog.locations.elapsedTime = glGetUniformLocation(prog.program, "elapsedTime");

    prog.initialized = true;

    return prog;
}

/**
 * \brief Load and compile shader programs. Get attribute locations.
 */
void loadShaderPrograms()
{
    commonShaderProgram = loadShaderProgram("simple-vs.glsl", "simple-fs.glsl");
    mandelrotShaderProgram = loadShaderProgram("simple-vs.glsl", "mandelbrot.frag");
}

/**
 * \brief Delete all shader program objects.
 */
void cleanupShaderPrograms(void) {

    pgr::deleteProgramAndShaders(commonShaderProgram.program);
    pgr::deleteProgramAndShaders(mandelrotShaderProgram.program);
}

/**
 * \brief Draw all scene objects.
 */
void drawScene(void) {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix =
        camera.getProjectionMatrix((float)gameState.windowWidth /
                                   (float)gameState.windowHeight);

    for (Object* object : objects) {
        if (object != nullptr)
            object->draw(viewMatrix, projectionMatrix);
    }
}

void updateCamera(float deltaTime) {
    camera.moveForward((gameState.keyMap[KEY_W]) * deltaTime);
    camera.moveBackward((gameState.keyMap[KEY_S]) * deltaTime);
    camera.moveRight((gameState.keyMap[KEY_D]) * deltaTime);
    camera.moveLeft((gameState.keyMap[KEY_A]) * deltaTime);
    camera.moveUp((gameState.keyMap[KEY_E]) * deltaTime);
    camera.moveDown((gameState.keyMap[KEY_Q]) * deltaTime);

    camera.rotate(cameraRotation.x * deltaTime,
                  cameraRotation.y * deltaTime);
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
    }
}

// Called whenever a key on the keyboard was released. The key is given by
// the "keyReleased" parameter, which is in ASCII. 
/**
 * \brief Handle the key released event.
 * \param keyReleased ASCII code of the released key
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY) {
    switch (keyReleased) {
        case 'w': case 'W': case 's': case 'S':
            gameState.keyMap[KEY_W] = false;
            gameState.keyMap[KEY_S] = false; break;
        case 'a': case 'A': case 'd': case 'D':
            gameState.keyMap[KEY_A] = false;
            gameState.keyMap[KEY_D] = false; break;
        case 'q': case 'Q': case 'e': case 'E':
            gameState.keyMap[KEY_Q] = false;
            gameState.keyMap[KEY_E] = false; break;
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
}

void specialKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY) {
} // key released

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
    
    ogMouseX = mouseX;
    ogMouseY = mouseY;
}

/**
 * \brief Handle mouse dragging (mouse move with any button pressed).
 *        This event follows the glutMouseFunc(mouseCb) event.
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void mouseMotionCb(int mouseX, int mouseY) {
    if (gameState.keyMap[RMB]) {
        int deltaX = (ogMouseX - mouseX);
        int deltaY = (ogMouseY - mouseY);

        camera.rotate(-deltaX * mouseSensitivity, deltaY * mouseSensitivity);

        ogMouseX = mouseX;
        ogMouseY = mouseY;
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
    const glm::mat4 sceneRootMatrix = glm::mat4(1.0f);
    
    // getting current time and time from last frame
    static float lastTime = 0.0f;
    float elapsedTime = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME)); // ms
    float deltaTime = elapsedTime - lastTime;
    lastTime = elapsedTime;

    // update time in the animated Mandelbrot shader
    glUniform1f(mandelrotShaderProgram.locations.elapsedTime, elapsedTime);

    // update the application state
    for (Object* object : objects) {
        if (object != nullptr)
            object->update(elapsedTime, &sceneRootMatrix);
    }
    updateCamera(deltaTime);

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

    // enable depth test and backface culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    objects.push_back(new SingleMesh(MODELS_PATH + (std::string)"shape.obj",
                                     &commonShaderProgram));
    SingleMesh *monke = new SingleMesh(MODELS_PATH + (std::string)"monke.obj",
                                       &commonShaderProgram);
    monke->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              glm::vec3(-2.0f, 0.0f, 1.0f)) *
                               glm::rotate(glm::mat4(1.0f),
                                           glm::radians(90.0f),
                                           glm::vec3(0.0f, 1.0f, 0.0f)));
    objects.push_back(monke);
    objects.push_back(new Square(&mandelrotShaderProgram));
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

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

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
        // glutSpecialFunc(specialKeyboardCb);     // key pressed
        // glutSpecialUpFunc(specialKeyboardUpCb); // key released
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
