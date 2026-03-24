#include "object.h"

Object::Object()
    : localModelMatrix(1.0f)
    , globalModelMatrix(1.0f) {
}

Object::~Object() = default;

void Object::updateGlobalModelMatrix(const glm::mat4* parentModelMatrix) {
    // if we have parent, multiply parent's matrix with ours
    if (parentModelMatrix != nullptr)
        globalModelMatrix = *parentModelMatrix * localModelMatrix;
    else
        globalModelMatrix = localModelMatrix;
}

void Object::updateChildren(float elapsedTime) {
    for (Object* child : children) {
        if (child != nullptr)
            child->update(elapsedTime, &globalModelMatrix);
    }
}

void Object::drawChildren(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    for (Object* child : children) {
        if (child != nullptr)
            child->draw(viewMatrix, projectionMatrix);
    }
}

void Object::addChild(Object* child) {
    if (child != nullptr)
        children.push_back(child);
}

void Object::setLocalModelMatrix(const glm::mat4& matrix) {
    localModelMatrix = matrix;
}

const glm::mat4& Object::getLocalModelMatrix() const {
    return localModelMatrix;
}

const glm::mat4& Object::getGlobalModelMatrix() const {
    return globalModelMatrix;
}

void Object::setPosition(const glm::vec3& position) {
    localModelMatrix[3] = glm::vec4(position, 1.0f);
}

glm::vec3 Object::getPosition() const {
    return glm::vec3(localModelMatrix[3]);
}

void Object::translate(const glm::vec3& offset) {
    localModelMatrix[3] += glm::vec4(offset, 0.0f);
}

void Object::update(float elapsedTime, const glm::mat4* parentModelMatrix) {
    updateGlobalModelMatrix(parentModelMatrix);
    updateChildren(elapsedTime);
}

void Object::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    drawChildren(viewMatrix, projectionMatrix);
}
