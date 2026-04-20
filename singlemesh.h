#pragma once

#include <string>
#include "renderableObject.h"

class SingleMesh : public RenderableObject {
public:
    SingleMesh(std::string modelFileName,
               ShaderProgram* shdrPrg = nullptr,
               const Material* mat = nullptr);
    ~SingleMesh();

    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
    void setBackfaceCullingOff(bool value);

private:
    bool initialized;  ///< object has the shader with defined locations
    bool backFaceCullingOff;

    bool loadSingleMesh(const std::string& fileName, ShaderProgram* shader, ObjectGeometry** geometry);
};
