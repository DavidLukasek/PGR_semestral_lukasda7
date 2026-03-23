#pragma once

#include "renderableObject.h"

class Triangle : public RenderableObject
{
public:

    Triangle(ShaderProgram* shdrPrg = nullptr);
    ~Triangle();

    void update(float elapsedTime, const glm::mat4* parentModelMatrix) override;
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:

    bool initialized;  ///< object has the shader with defined locations
};

