#pragma once

#include "object.h"

/**
 * \brief Shader program related stuff (id, locations, ...).
 */
typedef struct _ShaderProgram {
    /// identifier for the shader program
    GLuint program;

    bool initialized;

    /**
      * \brief Indices of the vertex shader inputs (locations)
      */
    struct {
        // vertex attributes locations
        GLint position;
        GLint texCoord;
        // uniforms locations
        GLint PVMmatrix;
        GLint elapsedTime;
    } locations;

    _ShaderProgram() : program(0), initialized(false) {
        locations.position = -1;
        locations.PVMmatrix = -1;
    }

} ShaderProgram;

/**
 * \brief Geometry of an object (vertices, triangles).
 */
typedef struct _ObjectGeometry {
    GLuint        vertexBufferObject;   ///< identifier for the vertex buffer object
    GLuint        elementBufferObject;  ///< identifier for the element buffer object
    GLuint        vertexArrayObject;    ///< identifier for the vertex array object
    unsigned int  numTriangles;         ///< number of triangles in the mesh
} ObjectGeometry;

/**
 * \brief Scene object that can be rendered using geometry and shader program.
 */
class RenderableObject : public Object {
protected:
    ObjectGeometry* geometry;
    ShaderProgram* shaderProgram;

public:
    explicit RenderableObject(ShaderProgram* shdrPrg = nullptr)
        : Object()
        , geometry(nullptr)
        , shaderProgram(shdrPrg) {
    }
    ~RenderableObject() override = default;
};
