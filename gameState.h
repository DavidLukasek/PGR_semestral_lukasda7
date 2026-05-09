//----------------------------------------------------------------------------------------
/**
 * \file       gameState.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Game state definitions.
 *
 *  Declares structures and enums representing runtime game state and high-level application flow.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include <glm/glm.hpp>

#include "config.h"

/**
 * \brief      Game state definitions.
 */
enum { KEY_W, KEY_A, KEY_S, KEY_D,
       KEY_Q, KEY_E, KEY_ESC, KEY_SHIFT,
       KEY_ARROW_UP, KEY_ARROW_DOWN,
       LMB, MMB, RMB, KEYS_COUNT };

/**
 * \brief      Game state definitions.
 */
struct CameraPresetTransform {
    glm::vec3 position;   ///< Camera position.
    glm::vec2 rotation;   ///< Camera rotation as yaw and pitch.
};

/// \brief Global array of three camera presets.
CameraPresetTransform cameraPresets[3] = {
    { CAMERA_PRESET_1_POSITION, CAMERA_PRESET_1_ROTATION },
    { CAMERA_PRESET_2_POSITION, CAMERA_PRESET_2_ROTATION },
    { CAMERA_PRESET_3_POSITION, CAMERA_PRESET_3_ROTATION }
};

/**
 * \brief      Game state definitions.
 */
typedef struct _GameState {
public:
    int windowWidth = WINDOW_WIDTH;       ///< Current window width.
    int windowHeight = WINDOW_HEIGHT;     ///< Current window height.

    int ogMouseX = -1;                    ///< Last cursor X position.
    int ogMouseY = -1;                    ///< Last cursor Y position.
    float mouseSensitivity = MOUSE_SENSITIVITY; ///< Mouse sensitivity for camera rotation.

    int currentCameraPresetIndex = 0;     ///< Active camera preset index.

    float elapsedTime = 0.0f;             ///< Elapsed time since application start.

    bool mandelbrotAnimStarted = false;   ///< Whether Mandelbrot animation was started.
    bool mandelbrotAnimPaused = false;    ///< Whether Mandelbrot animation is paused.
    float mandelbrotAnimStartTime = 0.0f; ///< Start or resume time of Mandelbrot animation.
    float mandelbrotAnimPauseTime = 0.0f; ///< Last pause timestamp of Mandelbrot animation.
    bool stageLightsAnimStarted = false;  ///< Whether stage light animation was initialized.
    bool stageLightsAnimRunning = false;  ///< Whether stage light animation is running.
    bool rocketFlamesEnabled = false;     ///< Whether rocket flame meshes are enabled.

    bool keyMap[KEYS_COUNT] = { false };  ///< Current input state table.

    glm::vec3 ambientColor = AMBIENT_COLOR; ///< Global ambient scene color.
} GameState;
