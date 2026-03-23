#pragma once

#include <vector>
#include "pgr.h"

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

class Object;
/**
 * \brief Linear representation of the scene objects.  The objects themselves may represent the subtrees.
 */
typedef std::vector<Object*> ObjectList;

/**
 * \brief Abstract base for all scene objects.
 */
class Object {
protected:
    glm::mat4   localModelMatrix;
    glm::mat4   globalModelMatrix;
    ObjectList  children;

public:
    Object() : localModelMatrix(1.0f), globalModelMatrix(1.0f) {}
    virtual ~Object() {}

    virtual void update(const float elapsedTime, const glm::mat4* parentModelMatrix) = 0;
    virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;
};
