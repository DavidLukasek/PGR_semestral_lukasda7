//----------------------------------------------------------------------------------------
/**
 * \file       square.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Square primitive declaration.
 *
 *  Declares geometry and interfaces for rendering a square primitive in the scene.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include "singlemesh.h"

/**
 * \brief      Square primitive declaration.
 */
class Square : public SingleMesh {
public:
    /// \brief Creates square with optional shader program.
    Square(ShaderProgram* shdrPrg = nullptr);
    /// \brief Releases square geometry.
    ~Square();
    /// \brief Draws square and children.
    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

    const int nVertices = 6;  ///< Number of square vertices.
private:
    bool initialized;                 ///< Whether square was initialized for rendering.
    static const float vertices[30];  ///< Interleaved vertex array with UV coordinates.

    /// \brief Creates VBO/VAO and binds vertex attributes.
    void initializeSquare();
};
