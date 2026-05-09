//----------------------------------------------------------------------------------------
/**
 * \file       object.cpp
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Scene object class implementation.
 *
 *  Implements base scene object behavior, including transform management and shared object logic.
 *
*/
//----------------------------------------------------------------------------------------
#include "object.h"

bool Object::suppressChildrenDraw = false;

Object::Object()
    : localModelMatrix(1.0f)
    , globalModelMatrix(1.0f)
    , objectID(-1)
    , visible(true) {
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

const ObjectList& Object::getChildren() const {
    return children;
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

void Object::setObjectID(unsigned char ID) {
    objectID = ID;
}

unsigned char Object::getObjectID() {
    return objectID;
}

void Object::setVisible(bool value) {
    visible = value;
}

bool Object::isVisible() const {
    return visible;
}

void Object::setSuppressChildrenDraw(bool suppress) {
    suppressChildrenDraw = suppress;
}

void Object::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (!visible)
        return;

    if (!suppressChildrenDraw)
        drawChildren(viewMatrix, projectionMatrix);
}
