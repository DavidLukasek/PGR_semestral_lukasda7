#pragma once

#include "../../renderableObject.h"

class Square : public RenderableObject
{
public:
    Square(ShaderProgram* shdrPrg = nullptr);
    ~Square();
    
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

    const int nVertices = 6;
private:
    bool initialized;
    static const float vertices[30];

    void initializeSquare();
};
