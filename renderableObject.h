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
        GLint normal;
        GLint texCoord;

        // uniform matrices locations
        GLint PVMmatrix;
        GLint viewMatrix;
        GLint modelMatrix;
        GLint normalMatrix;

        // uniform material locations
        GLint diffuse;
        GLint specular;
        GLint ambient;
        GLint shininess;

        // uniform misc locations
        GLint elapsedTime;
        GLint ambientColor;
        GLint cameraPosition;
        //GLint useTexture;
    } locations;

    _ShaderProgram() : program(0), initialized(false) {
        locations.position = -1;
        locations.normal = -1;
        locations.texCoord = -1;

        locations.PVMmatrix = -1;
        locations.modelMatrix = -1;
        locations.normalMatrix = -1;

        locations.diffuse = -1;
        locations.specular = -1;
        locations.ambient = -1;
        locations.shininess = -1;

        locations.elapsedTime = -1;
        locations.ambientColor = -1;
        locations.cameraPosition = -1;
    }

} ShaderProgram;

/**
 * \brief Geometry of an object (vertices, triangles).
 */
typedef struct _ObjectGeometry {
    GLuint        vertexBufferObject;   ///< identifier for the vertex buffer object
    GLuint        normalBufferObject;   ///< identifier for the normal buffer object
    GLuint        elementBufferObject;  ///< identifier for the element buffer object
    GLuint        vertexArrayObject;    ///< identifier for the vertex array object
    unsigned int  numTriangles;         ///< number of triangles in the mesh
} ObjectGeometry;

typedef struct _Material {  // structure that describes currently used material
    glm::vec3 ambient;      // ambient component
    glm::vec3 diffuse;      // diffuse component
    glm::vec3 specular;     // specular component
    float     shininess;    // sharpness of specular reflection
    //bool      useTexture;   // defines whether the texture is used or not
} Material;

/**
 * \brief Scene object that can be rendered using geometry and shader program.
 */
class RenderableObject : public Object {
protected:
    ObjectGeometry* geometry;
    ShaderProgram* shaderProgram;
    Material* material;

public:
    explicit RenderableObject(ShaderProgram* shdrPrg = nullptr,
                              Material* mat = nullptr)
        : Object()
        , geometry(nullptr)
        , shaderProgram(shdrPrg)
        , material(mat) {
    }
    ~RenderableObject() override = default;
};
