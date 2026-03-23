#pragma once

#include "object.h"

/**
 * \brief Scene object that can be rendered using geometry and shader program.
 */
class RenderableObject : public Object {
protected:
    ObjectGeometry* geometry;
    ShaderProgram* shaderProgram;
    
    void updateGlobalModelMatrix(const glm::mat4* parentModelMatrix);
    void updateChildren(const float elapsedTime);
    void drawChildren(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

public:
    RenderableObject(ShaderProgram* shdrPrg = nullptr)
        : Object()
        , geometry(nullptr)
        , shaderProgram(shdrPrg) {
    }
    ~RenderableObject() override {};

    void addChild(Object* child);

    void setLocalModelMatrix(const glm::mat4 matrix);

    const glm::mat4& getLocalModelMatrix() const;
    const glm::mat4& getGlobalModelMatrix() const;

    void update(const float elapsedTime, const glm::mat4* parentModelMatrix) override;
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
};
