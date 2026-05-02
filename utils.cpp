#include "utils.h"

#include <cmath>

#include "config.h"

glm::vec2 directionToYawPitch(const glm::vec3& direction) {
    const glm::vec3 dir = glm::normalize(direction);
    const float yaw = glm::degrees(std::atan2(dir.z, dir.x));
    const float pitch = glm::degrees(std::asin(glm::clamp(dir.y, -1.0f, 1.0f)));
    return glm::vec2(yaw, pitch);
}

void applyCameraPreset(Camera& camera,
                       const glm::vec3& presetPosition,
                       const glm::vec2& presetRotation) {
    
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
        // spaceship ignition case
        case 13:
            rocketFlamesEnabled = !rocketFlamesEnabled;
            if (rocketFlame1 != nullptr)
                rocketFlame1->setVisible(rocketFlamesEnabled);
            if (rocketFlame2 != nullptr)
                rocketFlame2->setVisible(rocketFlamesEnabled);
            break;
    }

    isLmbPressed = false;
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

