#pragma once

// ------------------------------- Application --------------------------------

#define SHADER_PATH "data/shaders/"
#define MODELS_PATH "data/models/"

#define WINDOW_TITLE  "PGR: Application lukasda7"
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

#define MAX_SCENE_LIGHTS    8    // also needs to be manually changed in phong.frag!
#define AMBIENT_COLOR       glm::vec3(0.05f)

#define GLOBAL_ANIM_SPEED   1.0f

#define MOUSE_SENSITIVITY   0.25f

// ---------------------------------- Camera ----------------------------------

#define MOVE_SPEED    50.0f
#define SPRINT_SPEED 100.0f
#define FOV_SPEED    100.0f
#define MIN_FOV       10.0f
#define MAX_FOV      170.0f
#define NEAR_PLANE     0.1f
#define FAR_PLANE   1000.0f

// ------------------------- Planetary system objects -------------------------

// object positions
#define UFO_POSITION       glm::vec3(0.0f, 15.0f, 0.0f)
#define MOON_POSITION      glm::vec3(0.0f, 0.0f, 200.0f)
#define PLANET_1_POSITION  glm::vec3(-160.0f, 80.0f, -260.0f)
#define PLANET_2_POSITION  glm::vec3(160.0f, 80.0f, 260.0f)
#define SUN_LOCATION       glm::vec3(1000.0f, 100.0f, -100.0f)

// object rotation speeds
#define UFO_AXIS_ROT_SPEED     -150.0f
#define UFO_ORBIT_ROT_SPEED      70.0f
#define MOON_AXIS_ROT_SPEED    -100.0f
#define MOON_ORBIT_ROT_SPEED     10.0f
#define PLANET_1_AXIS_ROT_SPEED  -5.0f
#define PLANET_2_AXIS_ROT_SPEED  15.0f
#define PLANET_ORBIT_ROT_SPEED    5.0f

// planet fog parameters
#define FOG_CENTER      PLANET_1_POSITION
#define FOG_COLOR       glm::vec3(0.64f, 0.62f, 0.9f)
#define FOG_RADIUS      105.0f
#define FOG_DENSITY       0.2f

// ------------------------------- Other objects ------------------------------

// object positions
#define SPACESHIP_POSITION     glm::vec3(-50.0f, 1.0f, 0.0f)
#define ROCKET_FLAME_1_OFFSET  glm::vec3(10.095f, 0.0, -33.345f)
#define ROCKET_FLAME_2_OFFSET  glm::vec3(-10.095f, 0.0, -33.345f)
