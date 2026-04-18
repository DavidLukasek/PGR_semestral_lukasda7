#pragma once

#include <glm/glm.hpp>

#define SHADER_PATH "data/shaders/"
#define MODELS_PATH "data/models/"

#define WINDOW_TITLE "PGR: Application lukasda7"

#define MAX_SCENE_LIGHTS 8

#define MOON_AXIS_ROTATION_SPEED -100.0f
#define MOON_PLANET_ROTATION_SPEED 20.0f
#define PLANET_AXIS_ROTATION_SPEED -5.0f

enum { KEY_W, KEY_A, KEY_S, KEY_D,
       KEY_Q, KEY_E, KEY_ESC, KEY_SHIFT,
       KEY_ARROW_UP, KEY_ARROW_DOWN,
       LMB, MMB, RMB, KEYS_COUNT };

typedef struct _GameState {
public:
    int windowWidth = 1920;
    int windowHeight = 1080;

    int ogMouseX = -1;
    int ogMouseY = -1;
    const float mouseSensitivity = 0.25f;

    float elapsedTime = 0.0f;

    bool mandelbrotAnimStarted = false;
    bool mandelbrotAnimPaused = false;
    float mandelbrotAnimStartTime = 0.0f;
    float mandelbrotAnimPauseTime = 0.0f;

    bool keyMap[KEYS_COUNT] = { false };

    glm::vec3 ambientColor = glm::vec3(0.05f);
} GameState;
