//----------------------------------------------------------------------------------------
/**
 * \file       utils.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Utility function declarations.
 *
 *  Declares helper functions shared across modules, such as math, loading and rendering utilities.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include <vector>

#include "camera.h"
#include "object.h"
#include "singlemesh.h"

typedef struct _ShaderProgram ShaderProgram;
class Light;

/**
 * \brief      Utility function declarations.
 */
struct CameraInputState {
    bool moveForward = false;    ///< Move forward.
    bool moveBackward = false;   ///< Move backward.
    bool moveRight = false;      ///< Move right.
    bool moveLeft = false;       ///< Move left.
    bool moveUp = false;         ///< Move up.
    bool moveDown = false;       ///< Move down.
    bool fovUpPressed = false;   ///< Decrease FOV.
    bool fovDownPressed = false; ///< Increase FOV.
    bool sprint = false;         ///< Sprint movement mode.
};

/// \brief Converts direction vector to camera yaw and pitch.
glm::vec2 directionToYawPitch(const glm::vec3& direction);

/// \brief Applies preset position and rotation to camera.
void applyCameraPreset(Camera& camera,
                       const glm::vec3& presetPosition,
                       const glm::vec2& presetRotation);

/// \brief Saves current camera transform into given preset slot.
void saveCurrentCameraToPreset(const Camera& camera,
                               glm::vec3& presetPosition,
                               glm::vec2& presetRotation);

/// \brief Updates third preset from tracked object and optionally applies it.
void updateCameraPreset3FromUfo(Camera& camera,
                                const Object* trackedObject,
                                glm::vec3& presetPosition,
                                glm::vec2& presetRotation,
                                int currentCameraPresetIndex);

/// \brief Updates camera from current user input.
void updateCameraFromInput(Camera& camera,
                           const CameraInputState& input,
                           float deltaTime);

/// \brief Clamps camera position to a sphere from inside or outside.
void collideCameraWithSphere(Camera& camera,
                             const glm::vec3& sphereCenter,
                             float sphereRadius,
                             bool inside);

/// \brief Recomputes preset transform from tracked object global transform.
void updatePresetFromTrackedObject(glm::vec3& presetPosition,
                                   glm::vec2& presetRotation,
                                   const Object* trackedObject);

/// \brief Recursively draws objects into stencil buffer for picking.
void drawObjectStencil(Object* object,
                       const glm::mat4& viewMatrix,
                       const glm::mat4& projectionMatrix);

/// \brief Prepares stencil buffer for object ID readback.
void drawStencil(Object* sceneRoot,
                 const glm::mat4& viewMatrix,
                 const glm::mat4& projectionMatrix);

/// \brief Returns object ID under cursor using stencil picking.
unsigned char pickObject(int mouseX,
                         int mouseY,
                         int windowHeight,
                         Object* sceneRoot,
                         const glm::mat4& viewMatrix,
                         const glm::mat4& projectionMatrix);

/// \brief Handles click interactions with scene objects.
void checkObjectPick(bool& isLmbPressed,
                     int mouseX,
                     int mouseY,
                     int windowHeight,
                     Object* sceneRoot,
                     const glm::mat4& viewMatrix,
                     const glm::mat4& projectionMatrix,
                     bool& mandelbrotAnimStarted,
                     bool& mandelbrotAnimPaused,
                     float& mandelbrotAnimStartTime,
                     float& mandelbrotAnimPauseTime,
                     float elapsedTime,
                     bool& stageLightsAnimStarted,
                     bool& stageLightsAnimRunning,
                     std::vector<SingleMesh*>& stageLights,
                     std::vector<SingleMesh*>& lightHolders,
                     bool& rocketFlamesEnabled,
                     SingleMesh* planet1,
                     Object* rocketFlame1,
                     Object* rocketFlame2);

/// \brief Updates stage light animation state.
void updateStageLightsAnimation(float deltaTime,
                                bool stageLightsAnimRunning,
                                std::vector<SingleMesh*>& stageLights,
                                std::vector<SingleMesh*>& lightHolders);

/// \brief Performs full camera update including collisions and tracked preset.
void updateCamera(float deltaTime,
                  Camera& camera,
                  const CameraInputState& input,
                  const glm::vec3& sceneCenter,
                  float sceneRadius,
                  const Object* collisionObject1,
                  float collisionRadius1,
                  const Object* collisionObject2,
                  float collisionRadius2,
                  const Object* trackedObject,
                  glm::vec3& trackedPresetPosition,
                  glm::vec2& trackedPresetRotation,
                  int currentCameraPresetIndex);

/// \brief Updates orbital and axis rotations in planetary system.
void rotatePlanetarySystem(float deltaTime,
                           Object* moon,
                           Object* ufo,
                           Object* planet1,
                           Object* planet2);

/// \brief Sets static uniforms for Mandelbrot shader.
void setMandelbrotStaticUniforms(const ShaderProgram& shaderProgram);

/// \brief Uploads fog parameters to selected shader program.
void setFogUniforms(const ShaderProgram& shaderProgram,
                    const glm::vec3& fogCenter,
                    const glm::vec3& fogCenter2,
                    const glm::vec3& fogColor,
                    const glm::vec3& fogColor2,
                    float fogRadius,
                    float fogRadius2,
                    float fogDensity,
                    float fogDensity2);

/// \brief Uploads all scene lights to shader.
void setLightUniforms(const ShaderProgram& shaderProgram,
                      const std::vector<Light*>& sceneLights);

/// \brief Uploads shared per-frame uniforms.
void setMiscUniforms(const ShaderProgram& shaderProgram,
                     float elapsedTime,
                     const glm::vec3& cameraPosition,
                     const glm::vec3& ambientColor);

/// \brief Binds environment map texture and sets its sampler.
void setEnvironmentMapUniforms(const ShaderProgram& shaderProgram,
                               GLuint environmentMapTextureObject);

/// \brief Uploads uniforms required by skydome rendering.
void setSkydomeUniforms(const ShaderProgram& shaderProgram,
                        const glm::vec3& cameraPosition);

/// \brief Uploads uniforms driving Mandelbrot animation.
void setMandelbrotAnimationUniforms(const ShaderProgram& shaderProgram,
                                    float elapsedTime,
                                    float mandelbrotAnimStartTime,
                                    bool mandelbrotAnimStarted,
                                    bool mandelbrotAnimPaused,
                                    float mandelbrotAnimPauseTime);

/// \brief Uploads uniforms for animated rocket flame shader.
void setRocketFlameUniforms(const ShaderProgram& shaderProgram,
                            const glm::vec3& cameraPosition,
                            float elapsedTime);

