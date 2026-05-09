//----------------------------------------------------------------------------------------
/**
 * \file       object.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Scene object class declaration.
 *
 *  Declares a base scene object abstraction with shared transform and rendering-related interfaces.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include "pgr.h"

class Object;
/**
 * \brief      Scene object class declaration.
 */
typedef std::vector<Object*> ObjectList;

/**
 * \brief      Scene object class declaration.
 */
class Object {
protected:
    glm::mat4       localModelMatrix;       ///< Local model matrix.
    glm::mat4       globalModelMatrix;      ///< Global model matrix after parent propagation.
    ObjectList      children;               ///< Child nodes in the scene graph.
    unsigned char   objectID;               ///< Object ID used for picking.
    bool            visible;                ///< Whether the object should be rendered.
    static bool     suppressChildrenDraw;   ///< Global flag for temporary child draw suppression.

    /// \brief Recomputes global matrix from parent transform.
    void updateGlobalModelMatrix(const glm::mat4* parentModelMatrix);
    /// \brief Updates all child nodes.
    void updateChildren(float elapsedTime);
    /// \brief Draws all child nodes.
    void drawChildren(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

public:
    /// \brief Creates a generic scene graph node.
    Object();
    /// \brief Virtual destructor.
    virtual ~Object();

    /// \brief Adds a child node.
    void addChild(Object* child);
    /// \brief Sets local model matrix.
    void setLocalModelMatrix(const glm::mat4& matrix);
    /// \brief Returns local model matrix.
    const glm::mat4& getLocalModelMatrix() const;
    /// \brief Returns global model matrix.
    const glm::mat4& getGlobalModelMatrix() const;
    /// \brief Returns constant reference to child list.
    const ObjectList& getChildren() const;
    /// \brief Sets position through local matrix translation part.
    void setPosition(const glm::vec3& position);
    /// \brief Returns current position from local matrix.
    glm::vec3 getPosition() const;
    /// \brief Applies translation offset to local matrix.
    void translate(const glm::vec3& offset);
    /// \brief Sets object ID used by picking.
    void setObjectID(unsigned char ID);
    /// \brief Returns object ID.
    unsigned char getObjectID();
    /// \brief Sets object visibility.
    void setVisible(bool value);
    /// \brief Returns whether object is visible.
    bool isVisible() const;
    /// \brief Enables or disables child drawing globally.
    static void setSuppressChildrenDraw(bool suppress);

    /// \brief Updates global transform and all children.
    virtual void update(float elapsedTime, const glm::mat4* parentModelMatrix);
    /// \brief Draws object and optionally its children.
    virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};
