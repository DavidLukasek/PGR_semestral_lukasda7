//----------------------------------------------------------------------------------------
/**
 * \file       sceneGraph.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Scene graph declarations.
 *
 *  Declares scene graph node structures and traversal-related interfaces for hierarchical scene management.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include <vector>

#include "../config.h"
#include "models/Item.h"
#include "../singlemesh.h"
#include "square.h"

/**
 * \brief      Scene graph declarations.
 */
typedef struct FogBall {
    glm::vec3 center = FOG_1_CENTER;  ///< Fog sphere center.
    glm::vec3 color  = FOG_1_COLOR;   ///< Fog color.
    float radius     = FOG_1_RADIUS;  ///< Fog influence radius.
    float density    = FOG_1_DENSITY; ///< Fog density.
} FogBall;

/// \brief Array of all shader programs used by the application.
std::vector<ShaderProgram> shaderPrograms;

/// \brief Shader program for Mandelbrot surface.
ShaderProgram mandelrotShaderProgram;
/// \brief Shader program for lit meshes.
ShaderProgram phongShaderProgram;
/// \brief Shader program for animated rocket flames.
ShaderProgram rocketFlameShaderProgram;
/// \brief Shader program for skydome rendering.
ShaderProgram skydomeShaderProgram;
/// \brief Shader program for displacement-rendered objects.
ShaderProgram displacementShaderProgram;

/// \brief Root node of the scene graph.
Object sceneRoot;

/// \brief Cache of scene light pointers for fast uniform updates.
std::vector<Light*> sceneLightsCache;
/// \brief Cache of stage light mesh nodes controlled by button 2.
std::vector<SingleMesh*> stageLightsCache;
/// \brief Cache of stage light holder nodes controlled by button 2.
std::vector<SingleMesh*> lightHoldersCache;

/// \brief Main scene camera.
Camera camera;
/// \brief Cached camera yaw/pitch rotation.
glm::vec2 cameraRotation = glm::vec2(0.0);

/// \brief Central shared application state.
GameState gameState;

/// \brief Texture object used as environment map.
GLuint environmentMapTextureObject = 0;

/// \brief Pointer to UFO mesh used in orbital animations.
SingleMesh* ufo           = nullptr;
/// \brief Pointer to first planet mesh for animation and interaction.
SingleMesh* planet1       = nullptr;
/// \brief Pointer to second planet mesh for animation and interaction.
SingleMesh* planet2       = nullptr;
/// \brief Pointer to cloud layer mesh of second planet.
SingleMesh* planet2Clouds = nullptr;
/// \brief Pointer to moon mesh in planetary system.
SingleMesh* moon          = nullptr;
/// \brief Pointer to first rocket flame mesh.
SingleMesh* rocketFlame1  = nullptr;
/// \brief Pointer to second rocket flame mesh.
SingleMesh* rocketFlame2  = nullptr;

/// \brief First fog sphere around planet 1.
FogBall fogBall1 = { FOG_1_CENTER, FOG_1_COLOR, FOG_1_RADIUS, FOG_1_DENSITY };
/// \brief Second fog sphere around planet 2.
FogBall fogBall2 = { FOG_2_CENTER, FOG_2_COLOR, FOG_2_RADIUS, FOG_2_DENSITY };

/// \brief Gray material for neutral meshes.
Material greyMaterial = {
    glm::vec3(0.3f),                // ambient
    glm::vec3(0.3f),                // diffuse
    glm::vec3(0.3f),                // specular
    10.0f                            // shininess
};

/// \brief White default material.
Material whiteMaterial = {
    glm::vec3(1.0f),                // ambient
    glm::vec3(1.0f),                // diffuse
    glm::vec3(0.23f, 0.20f, 0.11f), // specular
    10.0f                           // shininess
};

/// \brief Red material for button 1.
Material redMaterial = {
    glm::vec3(1.0f),                // ambient
    glm::vec3(1.0f, 0.0f, 0.0f),    // diffuse
    glm::vec3(0.93f, 0.20f, 0.11f), // specular
    10.0f                           // shininess
};

/// \brief Blue material for button 2.
Material blueMaterial = {
    glm::vec3(1.0f),                // ambient
    glm::vec3(0.0f, 0.0f, 1.0f),    // diffuse
    glm::vec3(0.23f, 0.20f, 0.91f), // specular
    10.0f                           // shininess
};

/**
 * \brief      Scene graph declarations.
 */
void createLights() {
    // point light
    Light* pointLight1 = new Light(POINT_LIGHT,         // light type
                                   glm::vec3(0.0f),     // ambient
                                   glm::vec3(1.0f),     // diffuse
                                   glm::vec3(1.0f));    // specular
    pointLight1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                    BUTTON_LIGHT_POSITION));
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

    // directional light
    Light* sunLight = new Light(DIRECTION_LIGHT,                // light type
                                glm::vec3(0.0f),                // ambient
                                glm::vec3(1.0f),                // diffuse
                                glm::vec3(1.0f),                // specular
                                glm::normalize(-SUN_LOCATION)); // light direction
    sceneRoot.addChild(sunLight);
    sceneLightsCache.push_back(sunLight);
}

/**
 * \brief      Scene graph declarations.
 * \param lightStandPosition Position of light stand base.
 * \param yRotate Rotation of whole hierarchy around y axis.
 * \param lightColor Spot light color.
 * \param objectIDs Incremental object ID generator.
 */
void createLightStandHierarchy(glm::vec3 lightStandPosition, float yRotate,
                               glm::vec3 lightColor, int& objectIDs) {
    // light stand
    SingleMesh* lightStand = new SingleMesh(MODELS_PATH + (std::string)"light_pole.obj",
                                            &phongShaderProgram,
                                            &whiteMaterial);
    lightStand->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                   lightStandPosition) *
                                    glm::rotate(glm::mat4(1.0f),
                                                glm::radians(yRotate),
                                                glm::vec3(0.0f, 1.0f, 0.0f)));
    lightStand->setObjectID(objectIDs++);
    sceneRoot.addChild(lightStand);

    // ------------------------------------------------------------------------

    // light holder
    SingleMesh* lightHolder = new SingleMesh(MODELS_PATH + (std::string)"light_holder.obj",
                                             &phongShaderProgram,
                                             &whiteMaterial);
    lightHolder->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                    LIGHT_HOLDER_OFFSET));
    lightHolder->setObjectID(objectIDs++);
    lightStand->addChild(lightHolder);
    lightHoldersCache.push_back(lightHolder);

    // ------------------------------------------------------------------------

    // stage light
    SingleMesh* stageLight = new SingleMesh(MODELS_PATH + (std::string)"light.obj",
                                            &phongShaderProgram,
                                            &whiteMaterial);
    stageLight->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                   LIGHT_OFFSET));
    stageLight->setObjectID(objectIDs++);
    lightHolder->addChild(stageLight);
    stageLightsCache.push_back(stageLight);
    // ------------------------------------------------------------------------

    // light front
    SingleMesh* lightFront = new SingleMesh(MODELS_PATH + (std::string)"light_front.obj",
                                            &phongShaderProgram,
                                            &whiteMaterial);
    lightFront->setObjectID(objectIDs++);
    stageLight->addChild(lightFront);

    // ------------------------------------------------------------------------

    // spot light
    Light* spotLight = new Light(SPOT_LIGHT,                  // light type
                                 glm::vec3(0.0f),             // ambient
                                 lightColor,                  // diffuse
                                 lightColor,                  // specular
                                 glm::vec3(0.0f, 1.0f, 0.0f), // spot direction
                                 cos(glm::radians(15.0f)),    // spot cutoff
                                 15.0f,                       // spot exponent
                                 30.0f);                      // light intensity
    lightFront->addChild(spotLight);
    sceneLightsCache.push_back(spotLight);
}

/**
 * \brief      Scene graph declarations.
 */
void createObjects() {
    // variable to assign all object's IDs
    int objectIDs = 1;

    // ------------------------------------------------------------------------

    // skydome
    SingleMesh* skydome = new SingleMesh(MODELS_PATH + (std::string)"skydome.obj",
                                         &skydomeShaderProgram,
                                         &whiteMaterial);
    skydome->setObjectID(objectIDs++);
    sceneRoot.addChild(skydome);

    // ------------------------------------------------------------------------

    // square with mandelbrot set
    Square* mandelbrot = new Square(&mandelrotShaderProgram);
    mandelbrot->setLocalModelMatrix(glm::translate(glm::mat4(1.0f), 
                                                   MANDELBROT_POSITION) *
                                    glm::rotate(glm::mat4(1.0f),
                                                glm::radians(MANDELBROT_Y_ROTATE),
                                                glm::vec3(0.0f, 1.0f, 0.0f)) *
                                    glm::scale(glm::mat4(1.0f),
                                               glm::vec3(2.0f)));
    mandelbrot->setObjectID(objectIDs++);
    sceneRoot.addChild(mandelbrot);

    // ------------------------------------------------------------------------

    // planet 1
    planet1 = new SingleMesh(MODELS_PATH + (std::string)"planet.obj",
                             &displacementShaderProgram,
                             &whiteMaterial);
    planet1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                               PLANET_1_POSITION));
    planet1->setDisplaceAnimated(false);
    planet1->setObjectID(objectIDs++);
    sceneRoot.addChild(planet1);

    // ------------------------------------------------------------------------

    // moon
    moon = new SingleMesh(MODELS_PATH + (std::string)"moon.obj",
                          &phongShaderProgram,
                          &whiteMaterial);
    moon->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                             MOON_POSITION));
    moon->setObjectID(objectIDs++);
    planet1->addChild(moon);

    // ------------------------------------------------------------------------

    // ufo
    ufo = new SingleMesh(MODELS_PATH + (std::string)"ufo.obj",
                         &phongShaderProgram,
                         &whiteMaterial);
    ufo->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                            UFO_POSITION));
    ufo->setObjectID(objectIDs++);
    moon->addChild(ufo);

    // ------------------------------------------------------------------------

    // planet 2
    planet2 = new SingleMesh(MODELS_PATH + (std::string)"planet2.obj",
                             &phongShaderProgram,
                             &whiteMaterial);
    planet2->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                               PLANET_2_POSITION));
    planet2->setObjectID(objectIDs++);
    sceneRoot.addChild(planet2);

    // ------------------------------------------------------------------------

    // planet 2 clouds
    planet2Clouds = new SingleMesh(MODELS_PATH + (std::string)"planet2_clouds.obj",
                                   &phongShaderProgram,
                                   &whiteMaterial);
    planet2Clouds->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                      PLANET_2_POSITION));
    planet2Clouds->setUVAnimated(true);
    planet2Clouds->setObjectID(objectIDs++);
    planet2Clouds->setBackfaceCullingOff(true);
    sceneRoot.addChild(planet2Clouds);

     // ------------------------------------------------------------------------

    // item
    SingleMesh* item = new Item(&phongShaderProgram);
    item->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                             ITEM_POSITION));
    item->setObjectID(objectIDs++);
    sceneRoot.addChild(item);

    // ------------------------------------------------------------------------

    // island
    SingleMesh* island = new SingleMesh(MODELS_PATH + (std::string)"island.obj",
                                        &phongShaderProgram,
                                        &whiteMaterial);
    island->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                               FLOOR_OFFSET));
    island->setObjectID(objectIDs++);
    sceneRoot.addChild(island);

    // ------------------------------------------------------------------------

    // island middle
    SingleMesh* islandMiddle = new SingleMesh(MODELS_PATH + (std::string)"island_middle.obj",
                                              &phongShaderProgram,
                                              &whiteMaterial);
    islandMiddle->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     FLOOR_OFFSET));
    islandMiddle->setObjectID(objectIDs++);
    sceneRoot.addChild(islandMiddle);

    // ------------------------------------------------------------------------

    // board stones
    SingleMesh* boardStones = new SingleMesh(MODELS_PATH + (std::string)"board_stones.obj",
                                             &phongShaderProgram,
                                             &whiteMaterial);
    boardStones->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                    BOARD_STONES_POSITION));
    boardStones->setObjectID(objectIDs++);
    sceneRoot.addChild(boardStones);

    // ------------------------------------------------------------------------

    // board
    SingleMesh* board = new SingleMesh(MODELS_PATH + (std::string)"board.obj",
                                       &phongShaderProgram,
                                       &whiteMaterial);
    board->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              BOARD_POSITION));
    board->setObjectID(objectIDs++);
    sceneRoot.addChild(board);

    // ------------------------------------------------------------------------

    // button stand 1
    SingleMesh* buttonStand1 = new SingleMesh(MODELS_PATH + (std::string)"button_stand.obj",
                                              &phongShaderProgram,
                                              &greyMaterial);
    buttonStand1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     BUTTON_STAND_1_POSITION));
    buttonStand1->setObjectID(objectIDs++);
    sceneRoot.addChild(buttonStand1);

    // ------------------------------------------------------------------------

    // button 1
    SingleMesh* button1 = new SingleMesh(MODELS_PATH + (std::string)"button.obj",
                                         &phongShaderProgram,
                                         &redMaterial);
    button1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                BUTTON_OFFSET));
    button1->setObjectID(objectIDs++);
    buttonStand1->addChild(button1);

    // ------------------------------------------------------------------------

    // button stand 2
    SingleMesh* buttonStand2 = new SingleMesh(MODELS_PATH + (std::string)"button_stand.obj",
                                              &phongShaderProgram,
                                              &greyMaterial);
    buttonStand2->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     BUTTON_STAND_2_POSITION));
    buttonStand2->setObjectID(objectIDs++);
    sceneRoot.addChild(buttonStand2);

    // ------------------------------------------------------------------------

    // button 2
    SingleMesh* button2 = new SingleMesh(MODELS_PATH + (std::string)"button.obj",
                                         &phongShaderProgram,
                                         &blueMaterial);
    button2->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                BUTTON_OFFSET));
    button2->setObjectID(objectIDs++);
    buttonStand2->addChild(button2);

    // ------------------------------------------------------------------------

    // light stands
    createLightStandHierarchy(LIGHTSTAND_1_POSITION, LIGHTSTAND_1_Y_ROTATE, LIGHT_1_COLOR, objectIDs);
    createLightStandHierarchy(LIGHTSTAND_2_POSITION, LIGHTSTAND_2_Y_ROTATE, LIGHT_2_COLOR, objectIDs);
    createLightStandHierarchy(LIGHTSTAND_3_POSITION, LIGHTSTAND_3_Y_ROTATE, LIGHT_3_COLOR, objectIDs);
    createLightStandHierarchy(LIGHTSTAND_4_POSITION, LIGHTSTAND_4_Y_ROTATE, LIGHT_4_COLOR, objectIDs);
    createLightStandHierarchy(LIGHTSTAND_5_POSITION, LIGHTSTAND_5_Y_ROTATE, LIGHT_5_COLOR, objectIDs);
    createLightStandHierarchy(LIGHTSTAND_6_POSITION, LIGHTSTAND_6_Y_ROTATE, LIGHT_6_COLOR, objectIDs);
    
    // ------------------------------------------------------------------------

    // spaceship
    SingleMesh* spaceship = new SingleMesh(MODELS_PATH + (std::string)"spaceship.obj",
                                           &phongShaderProgram,
                                           &whiteMaterial);
    spaceship->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                  SPACESHIP_POSITION) *
                                   glm::rotate(glm::mat4(1.0f),
                                               glm::radians(0.0f),
                                               glm::vec3(0.0f, 1.0f, 0.0f)));
    spaceship->setObjectID(objectIDs++);
    sceneRoot.addChild(spaceship);

    // ------------------------------------------------------------------------

    // rocket flame 1
    rocketFlame1 = new SingleMesh(MODELS_PATH + (std::string)"rocket_flame.obj",
                                  &rocketFlameShaderProgram,
                                  &whiteMaterial);
    rocketFlame1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     ROCKET_FLAME_1_OFFSET));
    rocketFlame1->setVisible(false);
    rocketFlame1->setBackfaceCullingOff(true);
    rocketFlame1->setAdditiveBlending(true);
    rocketFlame1->setObjectID(objectIDs++);
    spaceship->addChild(rocketFlame1);

    // ------------------------------------------------------------------------

    // rocket flame 2
    rocketFlame2 = new SingleMesh(MODELS_PATH + (std::string)"rocket_flame.obj",
                                  &rocketFlameShaderProgram,
                                  &whiteMaterial);
    rocketFlame2->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                                     ROCKET_FLAME_2_OFFSET));
    rocketFlame2->setVisible(false);
    rocketFlame2->setBackfaceCullingOff(true);
    rocketFlame2->setAdditiveBlending(true);
    rocketFlame2->setObjectID(objectIDs++);
    spaceship->addChild(rocketFlame2);
}
