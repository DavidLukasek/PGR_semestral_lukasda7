#pragma once

#define SHADER_PATH "data/shaders/"
#define MODELS_PATH "data/models/"

#define WINDOW_TITLE "PGR: Application lukasda7"

enum { KEY_W, KEY_A, KEY_S, KEY_D, KEY_Q, KEY_E, LMB, MMB, RMB, KEYS_COUNT };

typedef struct _GameState {
public:
    int windowWidth = 1920;
    int windowHeight = 1080;

    bool keyMap[KEYS_COUNT] = { false };
} GameState;
