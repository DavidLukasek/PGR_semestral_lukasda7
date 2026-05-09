//----------------------------------------------------------------------------------------
/**
 * \file       Item.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Hardcoded model item declaration.
 *
 *  Declares a hardcoded model item abstraction used to represent and manipulate imported model elements.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include "../../singlemesh.h"

/**
 * \brief      Model item declaration.
 */
class Item : public SingleMesh {
public:
    /// \brief Creates item and prepares its geometry.
    explicit Item(ShaderProgram* shdrPrg = nullptr);
    /// \brief Releases item geometry.
    ~Item() override;
    /// \brief Draws item and then children.
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:
    bool initialized;  ///< Whether item was initialized successfully.

    static const int nAttribsPerVertex = 8;  ///< Number of attributes per vertex.
    static const int nVertices = 250;        ///< Number of vertices.
    static const int nTriangles = 156;       ///< Number of triangles.
    static const float vertices[];           ///< Interleaved vertex data.
    static const unsigned triangles[];       ///< Triangle index buffer.

    /// \brief Creates VAO/VBO/EBO and binds item attributes.
    void initializeItem();
};
