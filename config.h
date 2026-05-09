//----------------------------------------------------------------------------------------
/**
 * \file       config.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Application configuration constants.
 *
 *  Defines global constants and compile-time configuration values shared across the project.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

// ############################################################################
//                                 Application
// ############################################################################

#define SHADER_PATH "data/shaders/"
#define MODELS_PATH "data/models/"

#define WINDOW_TITLE  "PGR: Application lukasda7"
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

#define MAX_SCENE_LIGHTS    10 // also needs to be manually changed in phong.frag!
#define AMBIENT_COLOR       glm::vec3(0.05f)

#define GLOBAL_ANIM_SPEED   0.5f

#define MOUSE_SENSITIVITY   0.25f

#define BOUNDING_SPHERE_RADIUS 500.0f

// ---------------------------------- Camera ----------------------------------

// basic camera settings
#define MOVE_SPEED     5.0f
#define SPRINT_SPEED  50.0f
#define FOV_SPEED    100.0f
#define MIN_FOV       10.0f
#define MAX_FOV      170.0f
#define NEAR_PLANE     0.1f
#define FAR_PLANE   1000.0f

// multiple camera preset positions and rotations
#define CAMERA_PRESET_1_POSITION glm::vec3(0.0f, 0.0f, 3.0f)
#define CAMERA_PRESET_1_ROTATION glm::vec2(-90.0f, 0.0f)      // yaw, pitch
#define CAMERA_PRESET_2_POSITION glm::vec3(40.0f, 25.0f, 40.0f)
#define CAMERA_PRESET_2_ROTATION glm::vec2(-135.0f, -20.0f)   // yaw, pitch
#define CAMERA_PRESET_3_POSITION glm::vec3(-80.0f, 40.0f, 0.0f)
#define CAMERA_PRESET_3_ROTATION glm::vec2(0.0f, -15.0f)      // yaw, pitch

// ------------------------- Mandelbrot set parameters ------------------------

#define MANDELBROT_MAX_ITERATIONS 1000
#define MANDELBROT_ZOOM_SPEED 1.0f
#define MANDELBROT_COLOR_SPEED 0.2f
#define MANDELBROT_ZOOM_TARGET glm::vec2(-1.186918, 0.300295)

// ############################################################################
//                                Scene objects
// ############################################################################

// ----------------------------- Planetary system -----------------------------

// planetary system object positions
#define UFO_POSITION       glm::vec3(0.0f, 15.0f, 0.0f)
#define MOON_POSITION      glm::vec3(0.0f, 0.0f, 200.0f)
#define PLANET_1_POSITION  glm::vec3(-160.0f, 80.0f, -260.0f)
#define PLANET_2_POSITION  glm::vec3(160.0f, 80.0f, 260.0f)
#define SUN_LOCATION       glm::vec3(-1.0f, 0.5f, 1.0f)

// planetary system object rotation speeds
#define UFO_AXIS_ROT_SPEED        -150.0f
#define UFO_ORBIT_ROT_SPEED         70.0f
#define MOON_AXIS_ROT_SPEED       -100.0f
#define MOON_ORBIT_ROT_SPEED        20.0f
#define PLANET_1_AXIS_ROT_SPEED     -5.0f
#define PLANET_2_AXIS_ROT_SPEED     15.0f
#define PLANET_ORBIT_ROT_SPEED       5.0f

#define PLANET_1_DISPLACEMENT_SIZE  10.0f

// planet fog parameters
#define FOG_1_CENTER      PLANET_1_POSITION
#define FOG_1_COLOR       glm::vec3(0.64f, 0.62f, 0.9f)
#define FOG_1_RADIUS      110.0f
#define FOG_1_DENSITY       0.2f
#define FOG_2_CENTER      PLANET_2_POSITION
#define FOG_2_COLOR       glm::vec3(0.5f, 0.5f, 0.95f)
#define FOG_2_RADIUS      105.0f
#define FOG_2_DENSITY       0.2f

// ------------------------------- Other objects ------------------------------

#define MANDELBROT_POSITION      glm::vec3(4.0741f, -0.7087f, -3.3598f)
#define MANDELBROT_Y_ROTATE      -56.3482f

#define SPACESHIP_POSITION       glm::vec3(50.0f, 1.0f, 0.0f)
#define ROCKET_FLAME_1_OFFSET    glm::vec3(10.095f, 0.0, -33.345f)
#define ROCKET_FLAME_2_OFFSET    glm::vec3(-10.095f, 0.0, -33.345f)

#define ITEM_POSITION            glm::vec3(0.0f, 20.0f, 0.0f)

#define FLOOR_OFFSET             glm::vec3(0.0f, -1.0f, 0.0f)

#define BOARD_STONES_POSITION    glm::vec3(0.0f, -1.0f, -5.2858f)
#define BOARD_POSITION           glm::vec3(0.0f, 0.29122f, -4.9681f)

#define BUTTON_STAND_1_POSITION  glm::vec3(-0.5f, -1.0f, 5.0f)
#define BUTTON_STAND_2_POSITION  glm::vec3(0.5f, -1.0f, 5.0f)
#define BUTTON_OFFSET            glm::vec3(0.0f, 1.07185f, 0.0f)

#define BUTTON_LIGHT_POSITION    glm::vec3(0.0f, 1.0f, 5.0f)

// stage lights
#define LIGHT_OFFSET             glm::vec3(0.0f, 0.286f, 0.0f)
#define LIGHT_LOCAL_X_ROTATION   -130.0f
#define LIGHT_SINE_STAGE_X_DEGREES   20.0f
#define LIGHT_SINE_HOLDER_Y_DEGREES  24.0f
#define LIGHT_SINE_SPEED_X           4.5f
#define LIGHT_SINE_SPEED_Y           3.6f
#define LIGHT_SINE_XY_PHASE_OFFSET   1.3472f
#define LIGHT_SINE_PER_LIGHT_OFFSET  0.7236f
#define LIGHTSTAND_1_POSITION    glm::vec3(-1.67f, -1.0f, 5.45f)
#define LIGHTSTAND_1_Y_ROTATE    -20.0f
#define LIGHT_1_COLOR            glm::vec3(1.0f, 0.0f, 0.0f)
#define LIGHTSTAND_2_POSITION    glm::vec3(1.67f, -1.0f, 5.45f)
#define LIGHTSTAND_2_Y_ROTATE    -(LIGHTSTAND_1_Y_ROTATE)
#define LIGHT_2_COLOR            glm::vec3(0.0f, 1.0f, 0.0f)
#define LIGHTSTAND_3_POSITION    glm::vec3(-3.646f, -1.0f, 4.386f)
#define LIGHTSTAND_3_Y_ROTATE    2*LIGHTSTAND_1_Y_ROTATE
#define LIGHT_3_COLOR            glm::vec3(0.0f, 0.0f, 1.0f)
#define LIGHTSTAND_4_POSITION    glm::vec3(3.646f, -1.0f, 4.386f)
#define LIGHTSTAND_4_Y_ROTATE    -(LIGHTSTAND_3_Y_ROTATE)
#define LIGHT_4_COLOR            glm::vec3(1.0f, 1.0f, 0.0f)
#define LIGHTSTAND_5_POSITION    glm::vec3(-5.045f, -1.0f, 2.666f)
#define LIGHTSTAND_5_Y_ROTATE    3*LIGHTSTAND_1_Y_ROTATE
#define LIGHT_5_COLOR            glm::vec3(1.0f, 0.0f, 1.0f)
#define LIGHTSTAND_6_POSITION    glm::vec3(5.045f, -1.0f, 2.666f)
#define LIGHTSTAND_6_Y_ROTATE    -(LIGHTSTAND_5_Y_ROTATE)
#define LIGHT_6_COLOR            glm::vec3(0.0f, 1.0f, 1.0f)
#define LIGHT_HOLDER_OFFSET      glm::vec3(0.0f, 3.0f, 0.0f)
