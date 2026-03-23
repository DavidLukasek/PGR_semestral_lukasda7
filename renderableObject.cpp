#include "renderableObject.h"

void RenderableObject::updateGlobalModelMatrix(const glm::mat4* parentModelMatrix) {
    // if we have parent, multiply parent's matrix with ours
    if (parentModelMatrix != nullptr)
        globalModelMatrix = *parentModelMatrix * localModelMatrix;
    else
        globalModelMatrix = localModelMatrix;
}

void RenderableObject::updateChildren(const float elapsedTime) {
    for (Object* child : children) {
        if (child != nullptr)
            child->update(elapsedTime, &globalModelMatrix);
    }
}

void RenderableObject::drawChildren(const glm::mat4& viewMatrix,
                                    const glm::mat4& projectionMatrix) {
    for (Object* child : children) {
        if (child != nullptr)
            child->draw(viewMatrix, projectionMatrix);
    }
}

void RenderableObject::setLocalModelMatrix(const glm::mat4 matrix) {
    localModelMatrix = matrix;
}

void RenderableObject::addChild(Object* child) {
    if (child != nullptr)
        children.push_back(child);
}

const glm::mat4& RenderableObject::getLocalModelMatrix() const {
    return localModelMatrix;
}

const glm::mat4& RenderableObject::getGlobalModelMatrix() const {
    return globalModelMatrix;
}

void RenderableObject::update(const float elapsedTime, const glm::mat4* parentModelMatrix) {
    updateGlobalModelMatrix(parentModelMatrix);
    updateChildren(elapsedTime);
}

void RenderableObject::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    drawChildren(viewMatrix, projectionMatrix);
}