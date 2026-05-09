//----------------------------------------------------------------------------------------
/**
 * \file       singlemesh.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Single mesh wrapper declaration.
 *
 *  Declares a lightweight mesh holder used to manage one drawable mesh and related resources.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include <string>
#include "renderableObject.h"

/**
 * \brief      Single mesh wrapper declaration.
 */
class SingleMesh : public RenderableObject {
public:
    /**
     * \brief Creates a mesh object and optionally loads it from a file.
     * \param modelFileName Path to model file or reserved name "square".
     * \param shdrPrg Shader program used for drawing.
     * \param mat Optional material.
     */
    SingleMesh(std::string modelFileName,
               ShaderProgram* shdrPrg = nullptr,
               const Material* mat = nullptr);
    /// \brief Releases GPU resources allocated for geometry.
    ~SingleMesh();

    /// \brief Draws mesh and then its children.
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
    /// \brief Returns displacement animation flag.
    bool getDisplaceAnimated();

    /// \brief Enables or disables back-face culling override.
    void setBackfaceCullingOff(bool value);
    /// \brief Enables or disables UV animation.
    void setUVAnimated(bool value);
    /// \brief Enables or disables displacement animation.
    void setDisplaceAnimated(bool value);
    /// \brief Enables or disables additive blending.
    void setAdditiveBlending(bool value);
    
private:
    bool initialized;           ///< Object has the shader with defined locations.
    bool backFaceCullingOff;    ///< Whether back-face culling should be disabled.
    bool isUVAnimated;          ///< Whether its UVs are animated.
    bool isDisplaceAnimated;    ///< Whether displacement animation is enabled.
    bool additiveBlending;      ///< Whether to use additive blending for this mesh.

    /**
     * \brief Loads one mesh from file and prepares VAO/VBO data.
     * \param fileName Model file path.
     * \param shader Shader program used for attribute mapping.
     * \param geometry Output geometry filled with mesh data.
     * \return True on successful load and initialization.
     */
    bool loadSingleMesh(const std::string& fileName, ShaderProgram* shader, ObjectGeometry** geometry);
};
