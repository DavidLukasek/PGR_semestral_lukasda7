#pragma once

#include "camera.h"
#include "object.h"
#include "singlemesh.h"

typedef struct _ShaderProgram ShaderProgram;
class Light;

struct CameraInputState {
    bool moveForward = false;
    bool moveBackward = false;
    bool moveRight = false;
    bool moveLeft = false;
    bool moveUp = false;
    bool moveDown = false;
    bool fovUpPressed = false;
    bool fovDownPressed = false;
    bool sprint = false;
};

glm::vec2 directionToYawPitch(const glm::vec3& direction);

void applyCameraPreset(Camera& camera,
                       const glm::vec3& presetPosition,
                       const glm::vec2& presetRotation);

void saveCurrentCameraToPreset(const Camera& camera,
                               glm::vec3& presetPosition,
                               glm::vec2& presetRotation);

void updateCameraPreset3FromUfo(Camera& camera,
                                const Object* trackedObject,
                                glm::vec3& presetPosition,
                                glm::vec2& presetRotation,
                                int currentCameraPresetIndex);

void updateCameraFromInput(Camera& camera,
                           const CameraInputState& input,
                           float deltaTime);

void collideCameraWithSphere(Camera& camera,
                             const glm::vec3& sphereCenter,
                             float sphereRadius,
                             bool inside);

void updatePresetFromTrackedObject(glm::vec3& presetPosition,
                                   glm::vec2& presetRotation,
                                   const Object* trackedObject);

void drawObjectStencil(Object* object,
                       const glm::mat4& viewMatrix,
                       const glm::mat4& projectionMatrix);

void drawStencil(Object* sceneRoot,
                 const glm::mat4& viewMatrix,
                 const glm::mat4& projectionMatrix);

unsigned char pickObject(int mouseX,
                         int mouseY,
                         int windowHeight,
                         Object* sceneRoot,
                         const glm::mat4& viewMatrix,
                         const glm::mat4& projectionMatrix);

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
                     bool& rocketFlamesEnabled,
                     SingleMesh* planet1,
                     Object* rocketFlame1,
                     Object* rocketFlame2);

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

void rotatePlanetarySystem(float deltaTime,
                           Object* moon,
                           Object* ufo,
                           Object* planet1,
                           Object* planet2);

void setMandelbrotStaticUniforms(const ShaderProgram& shaderProgram);

void setFogUniforms(const ShaderProgram& shaderProgram,
                    const glm::vec3& fogCenter,
                    const glm::vec3& fogCenter2,
                    const glm::vec3& fogColor,
                    const glm::vec3& fogColor2,
                    float fogRadius,
                    float fogRadius2,
                    float fogDensity,
                    float fogDensity2);

void setLightUniforms(const ShaderProgram& shaderProgram,
                      const std::vector<Light*>& sceneLights);

void setMiscUniforms(const ShaderProgram& shaderProgram,
                     float elapsedTime,
                     const glm::vec3& cameraPosition,
                     const glm::vec3& ambientColor);

void setEnvironmentMapUniforms(const ShaderProgram& shaderProgram,
                               GLuint environmentMapTextureObject);

void setSkydomeUniforms(const ShaderProgram& shaderProgram,
                        const glm::vec3& cameraPosition);

void setMandelbrotAnimationUniforms(const ShaderProgram& shaderProgram,
                                    float elapsedTime,
                                    float mandelbrotAnimStartTime,
                                    bool mandelbrotAnimStarted,
                                    bool mandelbrotAnimPaused,
                                    float mandelbrotAnimPauseTime);

void setRocketFlameUniforms(const ShaderProgram& shaderProgram,
                            const glm::vec3& cameraPosition,
                            float elapsedTime);

