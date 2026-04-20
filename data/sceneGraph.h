#pragma once

#include <vector>

#include "config.h"
#include "../singlemesh.h"
#include "square.h"

typedef struct FogBall {
    glm::vec3 center = FOG_CENTER;
    glm::vec3 color  = FOG_COLOR;
    float radius     = FOG_RADIUS;
    float density    = FOG_DENSITY;
} FogBall;

// array holding all shader programs in the application
std::vector<ShaderProgram> shaderPrograms;

ShaderProgram mandelrotShaderProgram;
ShaderProgram phongShaderProgram;
ShaderProgram rocketFlameShaderProgram;
ShaderProgram skydomeShaderProgram;

// root of the scene - all objects are children of it or of other its children
Object sceneRoot;

// cache storing references to all scene lights for faster updates
std::vector<Light*> sceneLightsCache;

// scene camera
Camera camera;
glm::vec2 cameraRotation = glm::vec2(0.0);

GameState gameState;

// holding on moon and planet references to rotate them later in rotateMoonAndPlanet
SingleMesh* ufo;
SingleMesh* planet;
SingleMesh* moon;

// fog ball
FogBall fogBall;

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

    // ufo spot light
    Light* ufoSpotLight = new Light(SPOT_LIGHT,                 // light type
                                  glm::vec3(0.0f),              // ambient
                                  glm::vec3(0.4f, 0.8f, 0.9f),  // diffuse
                                  glm::vec3(0.4f, 0.8f, 0.9f),  // specular
                                  glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                  cos(glm::radians(30.0f)),     // spot cutoff
                                  9.0f,                         // spot exponent
                                  50.0f);                       // light intensity
    ufo->addChild(ufoSpotLight);
    sceneLightsCache.push_back(ufoSpotLight);

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
    Light* sunLight = new Light(DIRECTION_LIGHT,                // light type
                                glm::vec3(0.0f),                // ambient
                                glm::vec3(1.0f),                // diffuse
                                glm::vec3(1.0f),                // specular
                                glm::normalize(-SUN_LOCATION)); // light direction
    sceneRoot.addChild(sunLight);
    sceneLightsCache.push_back(sunLight);
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
                                               PLANET_POSITION));
    sceneRoot.addChild(planet);

    // ------------------------------------------------------------------------

    // moon
    moon = new SingleMesh(MODELS_PATH + (std::string)"moon.obj",
                          &phongShaderProgram,
                          &material2);
    moon->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                             MOON_POSITION));
    planet->addChild(moon);

    // ------------------------------------------------------------------------

    // ufo
    ufo = new SingleMesh(MODELS_PATH + (std::string)"ufo.obj",
                         &phongShaderProgram,
                         &material2);
    ufo->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                            UFO_POSITION));
    moon->addChild(ufo);

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

    // spaceship
    SingleMesh* spaceship = new SingleMesh(MODELS_PATH + (std::string)"spaceship.obj",
                                           &phongShaderProgram,
                                           &material2);
    spaceship->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                  SPACESHIP_POSITION) *
                                   glm::rotate(glm::mat4(1.0f),
                                               glm::radians(0.0f),
                                               glm::vec3(0.0f, 1.0f, 0.0f)));
    sceneRoot.addChild(spaceship);

    // ------------------------------------------------------------------------

    // rocket flame 1
    SingleMesh* rocketFlame1 = new SingleMesh(MODELS_PATH + (std::string)"rocket_flame.obj",
                                              &rocketFlameShaderProgram,
                                              &material2);
    rocketFlame1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     ROCKET_FLAME_1_OFFSET));
    spaceship->addChild(rocketFlame1);

    // ------------------------------------------------------------------------

    // rocket flame 2
    SingleMesh* rocketFlame2 = new SingleMesh(MODELS_PATH + (std::string)"rocket_flame.obj",
                                              &rocketFlameShaderProgram,
                                              &material2);
    rocketFlame2->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     ROCKET_FLAME_2_OFFSET));
    spaceship->addChild(rocketFlame2);
}
