#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        float fieldOfViewDegrees = 90.0f,
        float nearPlane = 0.1f,
        float farPlane = 100.0f
    );

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void moveForward(float distance);
    void moveBackward(float distance);
    void moveRight(float distance);
    void moveLeft(float distance);
    void moveUp(float distance);
    void moveDown(float distance);

    void rotate(float yawOffsetDegrees, float pitchOffsetDegrees);

    void setPosition(const glm::vec3& newPosition);
    void setFieldOfView(float newFieldOfViewDegrees);

    const glm::vec3& getPosition() const;
    const glm::vec3& getFront() const;
    const glm::vec3& getUp() const;
    float getYaw() const;
    float getPitch() const;
    float getFieldOfView() const;

private:
    void updateVectors();
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;
    float fieldOfViewDegrees;
    float nearPlane;
    float farPlane;
};

