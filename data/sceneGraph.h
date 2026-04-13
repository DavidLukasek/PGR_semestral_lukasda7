#pragma once

#include <vector>

#include "../singlemesh.h"
#include "models/square.h"

typedef struct FogBall {
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 color  = glm::vec3(1.0f);
    float radius     = 10.0f;
    float density    = 0.1f;
} FogBall;

ShaderProgram commonShaderProgram;
ShaderProgram mandelrotShaderProgram;
ShaderProgram phongShaderProgram;
ShaderProgram rocketFlameShaderProgram;
ShaderProgram skydomeShaderProgram;

ObjectList objects;
std::vector<Light*> sceneLightsCache;

Camera camera;
glm::vec2 cameraRotation = glm::vec2(0.0);

GameState gameState;

glm::vec3 asteroidLocation = glm::vec3(0.0f, 0.0f, -15.0f);

// fog ball
FogBall fogBall = {
    asteroidLocation,               // fog center
    glm::vec3(0.64f, 0.62f, 0.9f),  // fog color
    13.0f,                          // fog radius
    0.07f                           // fog density
};

// grey material
Material material1 = {
    glm::vec3(0.3f),                // ambient
    glm::vec3(0.3f),                // diffuse
    glm::vec3(0.3f),                // specular
    2.0f                            // shininess
};

// white material
Material material2 = {
    glm::vec3(1.0f),                // ambient
    glm::vec3(1.0f),                // diffuse
    glm::vec3(0.23f, 0.20f, 0.11f), // specular
    2.0f                            // shininess
};

void createObjects() {
    // square with mandelbrot set
    objects.push_back(new Square(&mandelrotShaderProgram));

    // ------------------------------------------------------------------------

    // skydome
    SingleMesh* skydome = new SingleMesh(MODELS_PATH + (std::string)"skydome.obj",
                                         &skydomeShaderProgram,
                                         &material2);
    objects.push_back(skydome);

    // ------------------------------------------------------------------------

    // floor mesh
    SingleMesh* floor = new SingleMesh(MODELS_PATH + (std::string)"floor.obj",
                                       &phongShaderProgram,
                                       &material1);
    floor->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              glm::vec3(0.0f, -1.0f, 2.0f)));
    objects.push_back(floor);

    // ------------------------------------------------------------------------

    // Susanne
    SingleMesh* monke = new SingleMesh(MODELS_PATH + (std::string)"monke.obj",
                                       &phongShaderProgram,
                                       &material2);
    monke->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              glm::vec3(-2.0f, 0.0f, 2.0f)) *
                               glm::rotate(glm::mat4(1.0f),
                                           glm::radians(90.0f),
                                           glm::vec3(0.0f, 1.0f, 0.0f)));
    objects.push_back(monke);

    // ------------------------------------------------------------------------

    // point light
    Light* pointLight1 = new Light(POINT_LIGHT,         // light type
                                   glm::vec3(0.0f),     // ambient
                                   glm::vec3(1.0f),     // diffuse
                                   glm::vec3(1.0f));    // specular
    pointLight1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                     glm::vec3(-1.5f, 0.0f, 3.5f)));
    objects.push_back(pointLight1);

    // ------------------------------------------------------------------------

    // spot light 1
    Light* spotLight1 = new Light(SPOT_LIGHT,                   // light type
                                  glm::vec3(0.0f),              // ambient
                                  glm::vec3(1.0f, 0.0f, 0.0f),  // diffuse
                                  glm::vec3(1.0f, 0.0f, 0.0f),  // specular
                                  glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                  cos(glm::radians(45.0f)),     // spot cutoff
                                  5.0f,                         // spot exponent
                                  1.0f);                        // light intensity
    spotLight1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f)));
    objects.push_back(spotLight1);

    // ------------------------------------------------------------------------

    // spot light 2
    Light* spotLight2 = new Light(SPOT_LIGHT,                   // light type
                                  glm::vec3(0.0f),              // ambient
                                  glm::vec3(0.0f, 1.0f, 0.0f),  // diffuse
                                  glm::vec3(0.0f, 1.0f, 0.0f),  // specular
                                  glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                  cos(glm::radians(45.0f)),     // spot cutoff
                                  5.0f,                         // spot exponent
                                  1.0f);                        // light intensity
    spotLight2->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                    glm::vec3(-0.4f, 0.0f, 1.5f)));
    objects.push_back(spotLight2);

    // ------------------------------------------------------------------------

    // spot light 3
    Light* spotLight3 = new Light(SPOT_LIGHT,                   // light type
                                  glm::vec3(0.0f),              // ambient
                                  glm::vec3(0.0f, 0.0f, 1.0f),  // diffuse
                                  glm::vec3(0.0f, 0.0f, 1.0f),  // specular
                                  glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                  cos(glm::radians(45.0f)),     // spot cutoff
                                  5.0f,                         // spot exponent
                                  1.0f);                        // light intensity
    spotLight3->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                    glm::vec3(0.4f, 0.0f, 1.5f)));
    objects.push_back(spotLight3);

    // ------------------------------------------------------------------------

    // directional light 1
    Light* directionalLight1 = new Light(DIRECTION_LIGHT,               // light type
                                         glm::vec3(0.0f),               // ambient
                                         glm::vec3(0.5f),               // diffuse
                                         glm::vec3(1.0f),               // specular
                                         glm::vec3(0.0f, -1.0f, 0.2f)); // light direction
    objects.push_back(directionalLight1);

    // ------------------------------------------------------------------------

    // asteroid
    SingleMesh* asteroid = new SingleMesh(MODELS_PATH + (std::string)"asteroid.obj",
                                          &phongShaderProgram,
                                          &material2);
    asteroid->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                 asteroidLocation));
    objects.push_back(asteroid);

    // ------------------------------------------------------------------------

    // rocket flame
    SingleMesh* rocketFlame1 = new SingleMesh(MODELS_PATH + (std::string)"rocket_flame.obj",
                                              &rocketFlameShaderProgram,
                                              &material2);
    rocketFlame1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     glm::vec3(0.0, -1.0, 1.2)));
    objects.push_back(rocketFlame1);
}
