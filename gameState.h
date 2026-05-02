#pragma once

#include <glm/glm.hpp>

#include "config.h"

enum { KEY_W, KEY_A, KEY_S, KEY_D,
       KEY_Q, KEY_E, KEY_ESC, KEY_SHIFT,
       KEY_ARROW_UP, KEY_ARROW_DOWN,
       LMB, MMB, RMB, KEYS_COUNT };

struct CameraPresetTransform {
    glm::vec3 position;
    glm::vec2 rotation; // yaw, pitch
};

CameraPresetTransform cameraPresets[3] = {
    { CAMERA_PRESET_1_POSITION, CAMERA_PRESET_1_ROTATION },
    { CAMERA_PRESET_2_POSITION, CAMERA_PRESET_2_ROTATION },
    { CAMERA_PRESET_3_POSITION, CAMERA_PRESET_3_ROTATION }
};

typedef struct _GameState {
public:
    int windowWidth = WINDOW_WIDTH;
    int windowHeight = WINDOW_HEIGHT;

    int ogMouseX = -1;
    int ogMouseY = -1;
    float mouseSensitivity = MOUSE_SENSITIVITY;

    int currentCameraPresetIndex = 0;

    float elapsedTime = 0.0f;

    bool mandelbrotAnimStarted = false;
    bool mandelbrotAnimPaused = false;
    float mandelbrotAnimStartTime = 0.0f;
    float mandelbrotAnimPauseTime = 0.0f;
    bool rocketFlamesEnabled = false;

    bool keyMap[KEYS_COUNT] = { false };

    glm::vec3 ambientColor = AMBIENT_COLOR;
} GameState;
