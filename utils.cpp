#include "utils.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "config.h"
#include "light.h"
#include "renderableObject.h"

glm::vec2 directionToYawPitch(const glm::vec3& direction) {
    const glm::vec3 dir = glm::normalize(direction);
    const float yaw = glm::degrees(std::atan2(dir.z, dir.x));
    const float pitch = glm::degrees(std::asin(glm::clamp(dir.y, -1.0f, 1.0f)));
    return glm::vec2(yaw, pitch);
}

void applyCameraPreset(Camera& camera,
                       const glm::vec3& presetPosition,
                       const glm::vec2& presetRotation) {
    camera.setPosition(presetPosition);
    camera.setRotation(presetRotation.x, presetRotation.y);
}

void saveCurrentCameraToPreset(const Camera& camera,
                               glm::vec3& presetPosition,
                               glm::vec2& presetRotation) {
    presetPosition = camera.getPosition();
    presetRotation = glm::vec2(camera.getYaw(), camera.getPitch());
}

void updateCameraPreset3FromUfo(Camera& camera,
                                const Object* trackedObject,
                                glm::vec3& presetPosition,
                                glm::vec2& presetRotation,
                                int currentCameraPresetIndex) {
    updatePresetFromTrackedObject(presetPosition, presetRotation, trackedObject);

    if (currentCameraPresetIndex == 2)
        applyCameraPreset(camera, presetPosition, presetRotation);
}

void updateCameraFromInput(Camera& camera,
                           const CameraInputState& input,
                           float deltaTime) {
    // setting sprint
    camera.setSprinting(input.sprint);

    // branchless camera movement
    camera.moveForward(input.moveForward * deltaTime);
    camera.moveBackward(input.moveBackward * deltaTime);
    camera.moveRight(input.moveRight * deltaTime);
    camera.moveLeft(input.moveLeft * deltaTime);
    camera.moveUp(input.moveUp * deltaTime);
    camera.moveDown(input.moveDown * deltaTime);

    // branchless camera FOV change
    camera.changeFieldOfView(input.fovDownPressed * deltaTime -
                             input.fovUpPressed * deltaTime);
}

void collideCameraWithSphere(Camera& camera,
                             const glm::vec3& sphereCenter,
                             float sphereRadius,
                             bool inside) {
    glm::vec3 cameraPosition = camera.getPosition();
    glm::vec3 direction = cameraPosition - sphereCenter;

    float lengthSquared = glm::dot(direction, direction);

    // normalizing only non-zero vectors
    if (lengthSquared < 1e-6f) {
        direction = glm::vec3(0.0f, 1.0f, 0.0f);
        lengthSquared = 1.0f;
    }

    const float distance = std::sqrt(lengthSquared);

    // clamp either from inside or from outside depending on boolean value
    if (inside) {
        if (distance > sphereRadius)
            camera.setPosition(sphereCenter + direction / distance * sphereRadius);
    } else {
        if (distance < sphereRadius)
            camera.setPosition(sphereCenter + direction / distance * sphereRadius);
    }
}

void updatePresetFromTrackedObject(glm::vec3& presetPosition,
                                   glm::vec2& presetRotation,
                                   const Object* trackedObject) {
    if (trackedObject == nullptr)
        return;

    const glm::mat4& globalModelMatrix = trackedObject->getGlobalModelMatrix();
    presetPosition = glm::vec3(globalModelMatrix[3]);

    const glm::vec3 forwardDirection = glm::mat3(globalModelMatrix) *
                                       glm::vec3(0.0f, 0.0f, -1.0f);
    if (glm::length(forwardDirection) > 0.0001f)
        presetRotation = directionToYawPitch(forwardDirection);
}

void drawObjectStencil(Object* object,
                       const glm::mat4& viewMatrix,
                       const glm::mat4& projectionMatrix) {
    if ((object == nullptr) || !object->isVisible())
        return;

    glStencilFunc(GL_ALWAYS, object->getObjectID(), 0xFF);

    CHECK_GL_ERROR();

    object->draw(viewMatrix, projectionMatrix);

    for (Object* child : object->getChildren())
        drawObjectStencil(child, viewMatrix, projectionMatrix);
}

void drawStencil(Object* sceneRoot,
                 const glm::mat4& viewMatrix,
                 const glm::mat4& projectionMatrix) {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    CHECK_GL_ERROR();

    // draw object stencil recursive, therefore turn off children drawing due to duplicate render
    Object::setSuppressChildrenDraw(true);
    drawObjectStencil(sceneRoot, viewMatrix, projectionMatrix);
    Object::setSuppressChildrenDraw(false);

    glDisable(GL_STENCIL_TEST);
}

unsigned char pickObject(int mouseX,
                         int mouseY,
                         int windowHeight,
                         Object* sceneRoot,
                         const glm::mat4& viewMatrix,
                         const glm::mat4& projectionMatrix) {
    unsigned char objectID = 0;

    // draw object IDs into stencil buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    drawStencil(sceneRoot, viewMatrix, projectionMatrix);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glReadPixels(mouseX, windowHeight - mouseY - 1, 1, 1,
                 GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objectID);

    return objectID;
}

std::vector<glm::mat4> stageLightsBaseMatrices;
std::vector<glm::mat4> lightHoldersBaseMatrices;
float stageLightsAnimTime = 0.0f;
bool stageLightsBaseMatricesCaptured = false;
bool lightHoldersBaseMatricesCaptured = false;

void resetStageLightsAnimationState() {
    stageLightsBaseMatrices.clear();
    lightHoldersBaseMatrices.clear();
    stageLightsAnimTime = 0.0f;
    stageLightsBaseMatricesCaptured = false;
    lightHoldersBaseMatricesCaptured = false;
}

void rotateStageLightsLocally(std::vector<SingleMesh*>& stageLights,
                              float degrees,
                              const glm::vec3& axis) {
    const glm::mat4 localRotation = glm::rotate(glm::mat4(1.0f),
                                                glm::radians(degrees),
                                                axis);
    for (SingleMesh* stageLight : stageLights) {
        if (stageLight == nullptr)
            continue;

        stageLight->setLocalModelMatrix(stageLight->getLocalModelMatrix() * localRotation);
    }
}

void captureStageLightsBaseMatrices(std::vector<SingleMesh*>& stageLights) {
    stageLightsBaseMatrices.clear();
    stageLightsBaseMatrices.reserve(stageLights.size());
    for (SingleMesh* stageLight : stageLights) {
        if (stageLight == nullptr)
            stageLightsBaseMatrices.push_back(glm::mat4(1.0f));
        else
            stageLightsBaseMatrices.push_back(stageLight->getLocalModelMatrix());
    }
    stageLightsBaseMatricesCaptured = true;
}

void captureLightHoldersBaseMatrices(std::vector<SingleMesh*>& lightHolders) {
    lightHoldersBaseMatrices.clear();
    lightHoldersBaseMatrices.reserve(lightHolders.size());
    for (SingleMesh* lightHolder : lightHolders) {
        if (lightHolder == nullptr)
            lightHoldersBaseMatrices.push_back(glm::mat4(1.0f));
        else
            lightHoldersBaseMatrices.push_back(lightHolder->getLocalModelMatrix());
    }
    lightHoldersBaseMatricesCaptured = true;
}

void applyStageLightsSineAnimation(std::vector<SingleMesh*>& stageLights,
                                   std::vector<SingleMesh*>& lightHolders) {
    if (!stageLightsBaseMatricesCaptured || stageLightsBaseMatrices.size() != stageLights.size())
        captureStageLightsBaseMatrices(stageLights);
    if (!lightHoldersBaseMatricesCaptured || lightHoldersBaseMatrices.size() != lightHolders.size())
        captureLightHoldersBaseMatrices(lightHolders);

    for (size_t i = 0; i < stageLights.size(); ++i) {
        const float perLightPhaseOffset = static_cast<float>(i) * LIGHT_SINE_PER_LIGHT_OFFSET;
        const float xPhase = stageLightsAnimTime * LIGHT_SINE_SPEED_X + perLightPhaseOffset;
        const float yPhase = stageLightsAnimTime * LIGHT_SINE_SPEED_Y +
                             LIGHT_SINE_XY_PHASE_OFFSET + perLightPhaseOffset;
        const float xAngle = LIGHT_SINE_STAGE_X_DEGREES * std::sin(xPhase);
        const float yAngle = LIGHT_SINE_HOLDER_Y_DEGREES * std::sin(yPhase);

        const glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f),
                                                glm::radians(xAngle),
                                                glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f),
                                                glm::radians(yAngle),
                                                glm::vec3(0.0f, 1.0f, 0.0f));

        SingleMesh* stageLight = stageLights[i];
        if (stageLight != nullptr)
            stageLight->setLocalModelMatrix(stageLightsBaseMatrices[i] * xRotation);

        if (i < lightHolders.size()) {
            SingleMesh* lightHolder = lightHolders[i];
            if (lightHolder != nullptr)
                lightHolder->setLocalModelMatrix(lightHoldersBaseMatrices[i] * yRotation);
        }
    }
}

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
                     Object* rocketFlame2) {
    if (!isLmbPressed)
        return;

    const unsigned char objectID = pickObject(mouseX, mouseY, windowHeight,
                                              sceneRoot, viewMatrix, projectionMatrix);
    switch (objectID) {
        // mandelbrot animation case
        case 2:
            if (!mandelbrotAnimStarted) {
                mandelbrotAnimStarted = true;
                mandelbrotAnimPaused = false;
                mandelbrotAnimStartTime = elapsedTime;
                mandelbrotAnimPauseTime = elapsedTime;
            }
            else if (!mandelbrotAnimPaused) {
                mandelbrotAnimPaused = true;
                mandelbrotAnimPauseTime = elapsedTime;
            }
            else {
                mandelbrotAnimPaused = false;
                mandelbrotAnimStartTime += (elapsedTime - mandelbrotAnimPauseTime);
            }
            break;
        // button 1 -> planet 1 displacement toggle case
        case 14:
            planet1->setDisplaceAnimated(!(planet1->getDisplaceAnimated()));
            break;
        // button 2 -> stage lights animation
        case 16:
            if (!stageLightsAnimStarted) {
                rotateStageLightsLocally(stageLights,
                                         LIGHT_LOCAL_X_ROTATION,
                                         glm::vec3(1.0f, 0.0f, 0.0f));
                resetStageLightsAnimationState();
                captureStageLightsBaseMatrices(stageLights);
                captureLightHoldersBaseMatrices(lightHolders);
                stageLightsAnimStarted = true;
                stageLightsAnimRunning = true;
            }
            else
                stageLightsAnimRunning = !stageLightsAnimRunning;
            break;
        // spaceship ignition case
        case 41:
            rocketFlamesEnabled = !rocketFlamesEnabled;
            if (rocketFlame1 != nullptr)
                rocketFlame1->setVisible(rocketFlamesEnabled);
            if (rocketFlame2 != nullptr)
                rocketFlame2->setVisible(rocketFlamesEnabled);
            break;
    }

    isLmbPressed = false;
}

void updateStageLightsAnimation(float deltaTime,
                                bool stageLightsAnimRunning,
                                std::vector<SingleMesh*>& stageLights,
                                std::vector<SingleMesh*>& lightHolders) {
    if (!stageLightsAnimRunning)
        return;

    stageLightsAnimTime += deltaTime;
    applyStageLightsSineAnimation(stageLights, lightHolders);
}

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
                  int currentCameraPresetIndex) {
    // camera movement
    updateCameraFromInput(camera, input, deltaTime);

    // clamp camera to scene
    collideCameraWithSphere(camera, sceneCenter, sceneRadius, true);
    // collide with planet 1
    collideCameraWithSphere(camera, collisionObject1->getPosition(), collisionRadius1, false);
    // collide with planet 2
    collideCameraWithSphere(camera, collisionObject2->getPosition(), collisionRadius2, false);

    // update 3rd camera position/rotation preset to match UFo object
    updateCameraPreset3FromUfo(camera, trackedObject,
                               trackedPresetPosition, trackedPresetRotation,
                               currentCameraPresetIndex);
}

void rotatePlanetarySystem(float deltaTime,
                           Object* moon,
                           Object* ufo,
                           Object* planet1,
                           Object* planet2) {

    // ----------------------------- Moon rotation ----------------------------

    if (moon == nullptr || ufo == nullptr || planet1 == nullptr || planet2 == nullptr)
        return;

    static float moonOrbitAngle = 0.0f;
    static float moonAxisAngle = 0.0f;

    moonOrbitAngle += glm::radians(deltaTime *
                                   (MOON_ORBIT_ROT_SPEED - PLANET_1_AXIS_ROT_SPEED) *
                                   GLOBAL_ANIM_SPEED);
    moonAxisAngle += glm::radians(deltaTime *
                                  MOON_AXIS_ROT_SPEED *
                                  GLOBAL_ANIM_SPEED);

    glm::mat4 moonOrbit = glm::rotate(glm::mat4(1.0f),
                                      moonOrbitAngle,
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 moonOffset = glm::translate(glm::mat4(1.0f),
                                          MOON_POSITION);

    glm::mat4 moonSelfRotation = glm::rotate(glm::mat4(1.0f),
                                             moonAxisAngle,
                                             glm::vec3(0.0f, 1.0f, 0.0f));

    moon->setLocalModelMatrix(moonOrbit * moonOffset * moonSelfRotation);

    // ------------------------------ Ufo rotation ----------------------------

    static float ufoOrbitAngle = 0.0f;
    static float ufoAxisAngle = 0.0f;

    ufoOrbitAngle += glm::radians(deltaTime * UFO_ORBIT_ROT_SPEED * GLOBAL_ANIM_SPEED);
    ufoAxisAngle += glm::radians(deltaTime * UFO_AXIS_ROT_SPEED * GLOBAL_ANIM_SPEED);

    glm::mat4 ufoOrbit = glm::rotate(glm::mat4(1.0f),
                                     ufoOrbitAngle,
                                     glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 ufoOffset = glm::translate(glm::mat4(1.0f),
                                         UFO_POSITION);

    glm::mat4 ufoSelfRotation = glm::rotate(glm::mat4(1.0f),
                                            ufoAxisAngle,
                                            glm::vec3(0.0f, 1.0f, 0.0f));

    ufo->setLocalModelMatrix(glm::inverse(moonSelfRotation) * ufoOrbit *
                             ufoOffset * ufoSelfRotation);

    // --------------------------- Planets rotation ---------------------------

    static float planetsOrbitAngle = 0.0f;

    planetsOrbitAngle += glm::radians(deltaTime * PLANET_ORBIT_ROT_SPEED * GLOBAL_ANIM_SPEED);

    glm::mat4 planetsOrbit = glm::translate(glm::mat4(1.0f),
                                            glm::vec3(0.0f, 80.0f, 0.0f)) *
                             glm::rotate(glm::mat4(1.0f),
                                         planetsOrbitAngle,
                                         glm::vec3(0.0f, 1.0f, 0.0f)) *
                             glm::translate(glm::mat4(1.0f),
                                            glm::vec3(0.0f, -80.0f, 0.0f));

    static float planet1AxisAngle = 0.0f;

    planet1AxisAngle += glm::radians(deltaTime *
                                     (PLANET_1_AXIS_ROT_SPEED - PLANET_ORBIT_ROT_SPEED) *
                                     GLOBAL_ANIM_SPEED);

    glm::mat4 planet1Offset = glm::translate(glm::mat4(1.0f), PLANET_1_POSITION);
    glm::mat4 planet1SelfRotation = glm::rotate(glm::mat4(1.0f),
                                                planet1AxisAngle,
                                                glm::vec3(0.0f, 1.0f, 0.0f));

    planet1->setLocalModelMatrix(planetsOrbit * planet1Offset * planet1SelfRotation);

    static float planet2AxisAngle = 0.0f;

    planet2AxisAngle += glm::radians(deltaTime *
                                     (PLANET_2_AXIS_ROT_SPEED - PLANET_ORBIT_ROT_SPEED) *
                                     GLOBAL_ANIM_SPEED);

    glm::mat4 planet2Offset = glm::translate(glm::mat4(1.0f), PLANET_2_POSITION);
    glm::mat4 planet2SelfRotation = glm::rotate(glm::mat4(1.0f),
                                                planet2AxisAngle,
                                                glm::vec3(0.0f, 1.0f, 0.0f));

    planet2->setLocalModelMatrix(planetsOrbit * planet2Offset * planet2SelfRotation);
}

void setMandelbrotStaticUniforms(const ShaderProgram& shaderProgram) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    if (shaderProgram.locations.mandelbrotMaxIterations != -1)
        glUniform1i(shaderProgram.locations.mandelbrotMaxIterations,
                    MANDELBROT_MAX_ITERATIONS);

    if (shaderProgram.locations.mandelbrotZoomSpeed != -1)
        glUniform1f(shaderProgram.locations.mandelbrotZoomSpeed,
                    MANDELBROT_ZOOM_SPEED);

    if (shaderProgram.locations.mandelbrotColorSpeed != -1)
        glUniform1f(shaderProgram.locations.mandelbrotColorSpeed,
                    MANDELBROT_COLOR_SPEED);

    if (shaderProgram.locations.mandelbrotZoomTarget != -1)
        glUniform2fv(shaderProgram.locations.mandelbrotZoomTarget,
                     1, glm::value_ptr(MANDELBROT_ZOOM_TARGET));
}

void setFogUniforms(const ShaderProgram& shaderProgram,
                    const glm::vec3& fogCenter,
                    const glm::vec3& fogCenter2,
                    const glm::vec3& fogColor,
                    const glm::vec3& fogColor2,
                    float fogRadius,
                    float fogRadius2,
                    float fogDensity,
                    float fogDensity2) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    if (shaderProgram.locations.fogCenter != -1)
        glUniform3fv(shaderProgram.locations.fogCenter, 1, glm::value_ptr(fogCenter));

    if (shaderProgram.locations.fogCenter2 != -1)
        glUniform3fv(shaderProgram.locations.fogCenter2, 1, glm::value_ptr(fogCenter2));

    if (shaderProgram.locations.fogColor != -1)
        glUniform3fv(shaderProgram.locations.fogColor, 1, glm::value_ptr(fogColor));

    if (shaderProgram.locations.fogColor2 != -1)
        glUniform3fv(shaderProgram.locations.fogColor2, 1, glm::value_ptr(fogColor2));

    if (shaderProgram.locations.fogRadius != -1)
        glUniform1f(shaderProgram.locations.fogRadius, fogRadius);

    if (shaderProgram.locations.fogRadius2 != -1)
        glUniform1f(shaderProgram.locations.fogRadius2, fogRadius2);

    if (shaderProgram.locations.fogDensity != -1)
        glUniform1f(shaderProgram.locations.fogDensity, fogDensity);

    if (shaderProgram.locations.fogDensity2 != -1)
        glUniform1f(shaderProgram.locations.fogDensity2, fogDensity2);
}

void setLightUniforms(const ShaderProgram& shaderProgram,
                      const std::vector<Light*>& sceneLights) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    const GLint lightCountLocation = glGetUniformLocation(shaderProgram.program,
                                                          "lightCount");

    const int uploadedLightCount = std::min(static_cast<int>(sceneLights.size()),
                                            MAX_SCENE_LIGHTS);

    if (lightCountLocation != -1) glUniform1i(lightCountLocation, uploadedLightCount);

    // getting and setting all light-related uniform arrays
    for (int i = 0; i < uploadedLightCount; ++i) {
        const Light* light = sceneLights[i];
        const std::string lightIndex = std::to_string(i);
        const glm::vec3 lightPosition = glm::vec3(light->getGlobalModelMatrix()[3]);
        const glm::vec3 lightAmbient = light->getAmbient();
        const glm::vec3 lightDiffuse = light->getDiffuse();
        const glm::vec3 lightSpecular = light->getSpecular();
        const glm::vec3 lightSpotDirection = glm::normalize(
                                                glm::mat3(light->getGlobalModelMatrix())
                                                * light->getSpotDirection());
        const float lightSpotCutOff = light->getSpotCutOff();
        const float lightSpotExponent = light->getSpotExponent();
        const float lightIntensity = light->getIntensity();
        const int lightType = static_cast<int>(light->getLightType());

        const GLint typeLocation = glGetUniformLocation(
            shaderProgram.program, ("lightTypes[" + lightIndex + "]").c_str());

        const GLint ambientLocation = glGetUniformLocation(
            shaderProgram.program, ("lightAmbients[" + lightIndex + "]").c_str());

        const GLint diffuseLocation = glGetUniformLocation(
            shaderProgram.program, ("lightDiffuses[" + lightIndex + "]").c_str());

        const GLint specularLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpeculars[" + lightIndex + "]").c_str());

        const GLint positionLocation = glGetUniformLocation(
            shaderProgram.program, ("lightPositions[" + lightIndex + "]").c_str());

        const GLint spotDirectionLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpotDirections[" + lightIndex + "]").c_str());

        const GLint spotCutOffLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpotCutOffs[" + lightIndex + "]").c_str());

        const GLint spotExponentLocation = glGetUniformLocation(
            shaderProgram.program, ("lightSpotExponents[" + lightIndex + "]").c_str());

        const GLint intensityLocation = glGetUniformLocation(
            shaderProgram.program, ("lightIntensities[" + lightIndex + "]").c_str());

        if (typeLocation != -1)
            glUniform1i(typeLocation, lightType);

        if (ambientLocation != -1)
            glUniform3fv(ambientLocation, 1, glm::value_ptr(lightAmbient));

        if (diffuseLocation != -1)
            glUniform3fv(diffuseLocation, 1, glm::value_ptr(lightDiffuse));

        if (specularLocation != -1)
            glUniform3fv(specularLocation, 1, glm::value_ptr(lightSpecular));

        if (positionLocation != -1)
            glUniform3fv(positionLocation, 1, glm::value_ptr(lightPosition));

        if (spotDirectionLocation != -1)
            glUniform3fv(spotDirectionLocation, 1, glm::value_ptr(lightSpotDirection));

        if (spotCutOffLocation != -1)
            glUniform1f(spotCutOffLocation, lightSpotCutOff);

        if (spotExponentLocation != -1)
            glUniform1f(spotExponentLocation, lightSpotExponent);

        if (intensityLocation != -1)
            glUniform1f(intensityLocation, lightIntensity);
    }
}

void setMiscUniforms(const ShaderProgram& shaderProgram,
                     float elapsedTime,
                     const glm::vec3& cameraPosition,
                     const glm::vec3& ambientColor) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    // elapsed time uniform update
    if (shaderProgram.locations.elapsedTime != -1)
        glUniform1f(shaderProgram.locations.elapsedTime, elapsedTime);

    // camera position uniform update
    if (shaderProgram.locations.cameraPosition != -1) {
        glUniform3fv(shaderProgram.locations.cameraPosition,
                     1, glm::value_ptr(cameraPosition));
    }

    // ambient uniform update
    if (shaderProgram.locations.ambientColor != -1)
        glUniform3fv(shaderProgram.locations.ambientColor,
                     1, glm::value_ptr(ambientColor));
}

void setEnvironmentMapUniforms(const ShaderProgram& shaderProgram,
                               GLuint environmentMapTextureObject) {
    if (!shaderProgram.initialized || environmentMapTextureObject == 0)
        return;

    glUseProgram(shaderProgram.program);

    // environment map uniform update
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, environmentMapTextureObject);

    const GLint environmentSamplerLocation =
        glGetUniformLocation(shaderProgram.program, "environmentSampler");
    if (environmentSamplerLocation != -1)
        glUniform1i(environmentSamplerLocation, 3);

    glActiveTexture(GL_TEXTURE0);
}

void setSkydomeUniforms(const ShaderProgram& shaderProgram,
                        const glm::vec3& cameraPosition) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    if (shaderProgram.locations.cameraPosition != -1)
        glUniform3fv(shaderProgram.locations.cameraPosition,
                     1, glm::value_ptr(cameraPosition));
}

void setMandelbrotAnimationUniforms(const ShaderProgram& shaderProgram,
                                    float elapsedTime,
                                    float mandelbrotAnimStartTime,
                                    bool mandelbrotAnimStarted,
                                    bool mandelbrotAnimPaused,
                                    float mandelbrotAnimPauseTime) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    // update time and animation uniforms in the animated Mandelbrot shader
    if (shaderProgram.locations.elapsedTime != -1)
        glUniform1f(shaderProgram.locations.elapsedTime, elapsedTime);
    if (shaderProgram.locations.mandelbrotAnimStartTime != -1)
        glUniform1f(shaderProgram.locations.mandelbrotAnimStartTime,
                    mandelbrotAnimStartTime);
    if (shaderProgram.locations.mandelbrotAnimStarted != -1)
        glUniform1i(shaderProgram.locations.mandelbrotAnimStarted,
                    mandelbrotAnimStarted);
    if (shaderProgram.locations.mandelbrotAnimPaused != -1)
        glUniform1i(shaderProgram.locations.mandelbrotAnimPaused,
                    mandelbrotAnimPaused);
    if (shaderProgram.locations.mandelbrotAnimPauseTime != -1)
        glUniform1f(shaderProgram.locations.mandelbrotAnimPauseTime,
                    mandelbrotAnimPauseTime);
}

void setRocketFlameUniforms(const ShaderProgram& shaderProgram,
                            const glm::vec3& cameraPosition,
                            float elapsedTime) {
    if (!shaderProgram.initialized) return;
    glUseProgram(shaderProgram.program);

    // update time in the animated rocket flame shader
    if (shaderProgram.locations.cameraPosition != -1)
        glUniform3fv(shaderProgram.locations.cameraPosition,
                     1, glm::value_ptr(cameraPosition));
    if (shaderProgram.locations.elapsedTime != -1)
        glUniform1f(shaderProgram.locations.elapsedTime, elapsedTime);
}

