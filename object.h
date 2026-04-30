#pragma once

#include <vector>
#include "pgr.h"

class Object;
/**
 * \brief Linear representation of the scene objects.  The objects themselves may represent the subtrees.
 */
typedef std::vector<Object*> ObjectList;

/**
 * \brief Base for all scene objects.
 */
class Object {
protected:
    glm::mat4       localModelMatrix;
    glm::mat4       globalModelMatrix;
    ObjectList      children;
    unsigned char   objectID;
    bool            visible;
    static bool     suppressChildrenDraw;

    void updateGlobalModelMatrix(const glm::mat4* parentModelMatrix);
    void updateChildren(float elapsedTime);
    void drawChildren(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

public:
    Object();
    virtual ~Object();

    void addChild(Object* child);
    void setLocalModelMatrix(const glm::mat4& matrix);
    const glm::mat4& getLocalModelMatrix() const;
    const glm::mat4& getGlobalModelMatrix() const;
    const ObjectList& getChildren() const;
    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;
    void translate(const glm::vec3& offset);
    void setObjectID(unsigned char ID);
    unsigned char getObjectID();
    void setVisible(bool value);
    bool isVisible() const;
    static void setSuppressChildrenDraw(bool suppress);

    virtual void update(float elapsedTime, const glm::mat4* parentModelMatrix);
    virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};
