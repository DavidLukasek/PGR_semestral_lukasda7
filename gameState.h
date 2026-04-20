#pragma once

#include <glm/glm.hpp>

#include "config.h"

enum { KEY_W, KEY_A, KEY_S, KEY_D,
       KEY_Q, KEY_E, KEY_ESC, KEY_SHIFT,
       KEY_ARROW_UP, KEY_ARROW_DOWN,
       LMB, MMB, RMB, KEYS_COUNT };

typedef struct _GameState {
public:
    int windowWidth = WINDOW_WIDTH;
    int windowHeight = WINDOW_HEIGHT;

    int ogMouseX = -1;
    int ogMouseY = -1;
    float mouseSensitivity = MOUSE_SENSITIVITY;

    float elapsedTime = 0.0f;

    bool mandelbrotAnimStarted = false;
    bool mandelbrotAnimPaused = false;
    float mandelbrotAnimStartTime = 0.0f;
    float mandelbrotAnimPauseTime = 0.0f;

    bool keyMap[KEYS_COUNT] = { false };

    glm::vec3 ambientColor = AMBIENT_COLOR;
} GameState;
