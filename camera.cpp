#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(
    glm::vec3 position,
    float fieldOfViewDegrees,
    float nearPlane,
    float farPlane
)
    : position(position)
    , worldUp(0.0f, 1.0f, 0.0f)
    , yaw(-90.0f)
    , pitch(0.0f)
    , fieldOfViewDegrees(fieldOfViewDegrees)
    , nearPlane(nearPlane)
    , farPlane(farPlane) {
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fieldOfViewDegrees), aspectRatio, nearPlane, farPlane);
}

void Camera::moveForward(float distance) {
    position += front * distance;
}

void Camera::moveBackward(float distance) {
    position -= front * distance;
}

void Camera::moveRight(float distance) {
    position += right * distance;
}

void Camera::moveLeft(float distance) {
    position -= right * distance;
}

void Camera::moveUp(float distance) {
    position += worldUp * distance;
}

void Camera::moveDown(float distance) {
    position -= worldUp * distance;
}

void Camera::rotate(float yawOffsetDegrees, float pitchOffsetDegrees) {
    yaw += yawOffsetDegrees;
    pitch = glm::clamp(pitch + pitchOffsetDegrees, -89.0f, 89.0f);
    updateVectors();
}

void Camera::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

void Camera::setFieldOfView(float newFieldOfViewDegrees) {
    fieldOfViewDegrees = glm::clamp(newFieldOfViewDegrees, 1.0f, 120.0f);
}

const glm::vec3& Camera::getPosition() const {
    return position;
}

const glm::vec3& Camera::getFront() const {
    return front;
}

const glm::vec3& Camera::getUp() const {
    return up;
}

float Camera::getYaw() const {
    return yaw;
}

float Camera::getPitch() const {
    return pitch;
}

float Camera::getFieldOfView() const {
    return fieldOfViewDegrees;
}

void Camera::updateVectors() {
    const float yawRadians = glm::radians(yaw);
    const float pitchRadians = glm::radians(pitch);

    glm::vec3 direction;
    direction.x = cos(yawRadians) * cos(pitchRadians);
    direction.y = sin(pitchRadians);
    direction.z = sin(yawRadians) * cos(pitchRadians);

    front = glm::normalize(direction);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}
