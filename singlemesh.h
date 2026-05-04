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
    bool getDisplaceAnimated();

    void setBackfaceCullingOff(bool value);
    void setUVAnimated(bool value);
    void setDisplaceAnimated(bool value);
    void setAdditiveBlending(bool value);
    
private:
    bool initialized;           ///< object has the shader with defined locations
    bool backFaceCullingOff;
    bool isUVAnimated;          ///< whether its UVs are animated
    bool isDisplaceAnimated;    ///< whether displacement animation is enabled
    bool additiveBlending;      ///< whether to use additive blending for this mesh

    bool loadSingleMesh(const std::string& fileName, ShaderProgram* shader, ObjectGeometry** geometry);
};
