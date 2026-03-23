#pragma once

#include "../renderableObject.h"

class Square : public RenderableObject
{
public:
    Square(ShaderProgram* shdrPrg = nullptr);
    ~Square();
    
    void update(float elapsedTime, const glm::mat4* parentModelMatrix) override;
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

    const int nVertices = 6;
private:
    bool initialized;
    static const float vertices[30];

    void initializeSquare();
};
