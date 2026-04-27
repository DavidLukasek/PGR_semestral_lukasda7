#pragma once

#include "../../singlemesh.h"

class Item : public SingleMesh {
public:
    explicit Item(ShaderProgram* shdrPrg = nullptr);
    ~Item() override;
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:
    bool initialized;

    static const int nAttribsPerVertex = 8;
    static const int nVertices = 250;
    static const int nTriangles = 156;
    static const float vertices[];
    static const unsigned triangles[];

    void initializeItem();
};
