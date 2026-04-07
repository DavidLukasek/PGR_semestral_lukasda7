#pragma once

#include <glm/glm.hpp>

#define SHADER_PATH "data/shaders/"
#define MODELS_PATH "data/models/"

#define WINDOW_TITLE "PGR: Application lukasda7"

#define MAX_SCENE_LIGHTS 16

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

    bool keyMap[KEYS_COUNT] = { false };

    glm::vec3 ambientColor = glm::vec3(0.05f);
} GameState;
