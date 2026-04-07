#include "camera.h"

Camera::Camera(
    glm::vec3 initialPosition,
    float fieldOfViewDegrees,
    float nearPlane,
    float farPlane,
    bool isSprinting
)
    : worldUp(0.0f, 1.0f, 0.0f)
    , yaw(-90.0f)
    , pitch(0.0f)
    , fieldOfViewDegrees(fieldOfViewDegrees)
    , nearPlane(nearPlane)
    , farPlane(farPlane)
    , isSprinting(isSprinting) {
    setPosition(initialPosition);
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    const glm::vec3 position = getPosition();
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fieldOfViewDegrees), aspectRatio, nearPlane, farPlane);
}

void Camera::moveForward(float distance) {
    float speed = isSprinting ? SPRINT_SPEED : MOVE_SPEED;
    translate(front * distance * speed);
}

void Camera::moveBackward(float distance) {
    float speed = isSprinting ? SPRINT_SPEED : MOVE_SPEED;
    translate(-front * distance * speed);
}

void Camera::moveRight(float distance) {
    float speed = isSprinting ? SPRINT_SPEED : MOVE_SPEED;
    translate(right * distance * speed);
}

void Camera::moveLeft(float distance) {
    float speed = isSprinting ? SPRINT_SPEED : MOVE_SPEED;
    translate(-right * distance * speed);
}

void Camera::moveUp(float distance) {
    float speed = isSprinting ? SPRINT_SPEED : MOVE_SPEED;
    translate(worldUp * distance * speed);
}

void Camera::moveDown(float distance) {
    float speed = isSprinting ? SPRINT_SPEED : MOVE_SPEED;
    translate(-worldUp * distance * speed);
}

void Camera::rotate(float yawOffsetDegrees, float pitchOffsetDegrees) {
    yaw += yawOffsetDegrees;
    pitch = glm::clamp(pitch + pitchOffsetDegrees, -89.0f, 89.0f);
    updateVectors();
}

void Camera::changeFieldOfView(float delta) {
    fieldOfViewDegrees = glm::clamp(fieldOfViewDegrees + FOV_SPEED * delta,
                                    MIN_FOV, MAX_FOV);
}

void Camera::setSprinting(bool value) {
    isSprinting = value;
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
