#pragma once

#include <vector>

#include "../singlemesh.h"
#include "square.h"

typedef struct FogBall {
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 color  = glm::vec3(1.0f);
    float radius     = 10.0f;
    float density    = 0.1f;
} FogBall;

// array holding all shader programs in the application
std::vector<ShaderProgram> shaderPrograms;

ShaderProgram mandelrotShaderProgram;
ShaderProgram phongShaderProgram;
ShaderProgram rocketFlameShaderProgram;
ShaderProgram skydomeShaderProgram;

// root of the scene - all objects are children of it or of other its children
Object sceneRoot;
std::vector<Light*> sceneLightsCache;

// scene camera
Camera camera;
glm::vec2 cameraRotation = glm::vec2(0.0);

GameState gameState;

// planet and moon locations to be used for fog and rotations laterin lukasda7.cpp
glm::vec3 planetPosition = glm::vec3(-120.0f, 80.0f, -220.0f);
glm::vec3 moonPosition = glm::vec3(0.0f, 0.0f, 200.0f);

// holding on moon and planet references to rotate them later in rotateMoonAndPlanet
SingleMesh* planet;
SingleMesh* moon;

// fog ball
FogBall fogBall = {
    planetPosition,                 // fog center
    glm::vec3(0.64f, 0.62f, 0.9f),  // fog color
    105.0f,                         // fog radius
    0.2f                            // fog density
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

void createLights() {
    // point light
    Light* pointLight1 = new Light(POINT_LIGHT,         // light type
                                   glm::vec3(0.0f),     // ambient
                                   glm::vec3(1.0f),     // diffuse
                                   glm::vec3(1.0f));    // specular
    pointLight1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                    glm::vec3(-1.5f, 0.0f, 3.5f)));
    sceneRoot.addChild(pointLight1);
    sceneLightsCache.push_back(pointLight1);

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
    sceneRoot.addChild(spotLight1);
    sceneLightsCache.push_back(spotLight1);

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
    sceneRoot.addChild(spotLight2);
    sceneLightsCache.push_back(spotLight2);

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
    sceneRoot.addChild(spotLight3);
    sceneLightsCache.push_back(spotLight3);

    // ------------------------------------------------------------------------

    // directional light
    Light* directionalLight = new Light(DIRECTION_LIGHT,                 // light type
                                         glm::vec3(0.0f),                // ambient
                                         glm::vec3(1.0f),                // diffuse
                                         glm::vec3(1.0f),                // specular
                                         glm::vec3(-1.0f, -0.1f, 0.1f)); // light direction
    sceneRoot.addChild(directionalLight);
    sceneLightsCache.push_back(directionalLight);
}

void createObjects() {
    // square with mandelbrot set
    sceneRoot.addChild(new Square(&mandelrotShaderProgram));

    // ------------------------------------------------------------------------

    // skydome
    SingleMesh* skydome = new SingleMesh(MODELS_PATH + (std::string)"skydome.obj",
                                         &skydomeShaderProgram,
                                         &material2);
    sceneRoot.addChild(skydome);

    // ------------------------------------------------------------------------

    // planet
    planet = new SingleMesh(MODELS_PATH + (std::string)"planet.obj",
                            &phongShaderProgram,
                            &material2);
    planet->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                               planetPosition) *
                                glm::rotate(glm::mat4(1.0f),
                                            glm::radians(90.0f),
                                            glm::vec3(0.0f, 1.0f, 0.0f)));
    sceneRoot.addChild(planet);

    // ------------------------------------------------------------------------

    // moon
    moon = new SingleMesh(MODELS_PATH + (std::string)"moon.obj",
                          &phongShaderProgram,
                          &material2);
    moon->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                             moonPosition));
    planet->addChild(moon);

    // ------------------------------------------------------------------------

    // floor mesh
    SingleMesh* floor = new SingleMesh(MODELS_PATH + (std::string)"floor.obj",
                                       &phongShaderProgram,
                                       &material1);
    floor->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              glm::vec3(0.0f, -1.0f, 2.0f)));
    sceneRoot.addChild(floor);
    
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
    sceneRoot.addChild(monke);

    // ------------------------------------------------------------------------

    // rocket flame
    SingleMesh* rocketFlame1 = new SingleMesh(MODELS_PATH + (std::string)"rocket_flame.obj",
                                              &rocketFlameShaderProgram,
                                              &material2);
    rocketFlame1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     glm::vec3(0.0, -1.0, 1.2)));
    sceneRoot.addChild(rocketFlame1);
}
