#pragma once

#include "object.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera : public Object {
public:
    static constexpr float MOVE_SPEED = 50.0f;
    static constexpr float SPRINT_SPEED = 100.0f;
    static constexpr float FOV_SPEED = 100.0f;
    static constexpr float MIN_FOV = 10.0f;
    static constexpr float MAX_FOV = 170.0f;

    Camera(
        glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 3.0f),
        float fieldOfViewDegrees = 90.0f,
        float nearPlane = 0.1f,
        float farPlane = 1000.0f,
        bool isSprinting = false
    );
    ~Camera() override = default;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void moveForward(float distance);
    void moveBackward(float distance);
    void moveRight(float distance);
    void moveLeft(float distance);
    void moveUp(float distance);
    void moveDown(float distance);

    void rotate(float yawOffsetDegrees, float pitchOffsetDegrees);

    void changeFieldOfView(float delta);
    void setSprinting(bool value);

    const glm::vec3& getFront() const;
    const glm::vec3& getUp() const;
    float getYaw() const;
    float getPitch() const;
    float getFieldOfView() const;

private:
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;
    float fieldOfViewDegrees;
    float nearPlane;
    float farPlane;
    bool isSprinting;

    void updateVectors();
};
