#pragma once

#include "object.h"

class SingleMesh : public ObjectInstance
{
public:

    SingleMesh(ShaderProgram* shdrPrg = nullptr);
    ~SingleMesh();

    void update(float elapsedTime, const glm::mat4* parentModelMatrix) override;
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:

    bool loadSingleMesh(const std::string& fileName, ShaderProgram* shader, ObjectGeometry** geometry);

    bool initialized;  ///< object has the shader with defined locations
};

