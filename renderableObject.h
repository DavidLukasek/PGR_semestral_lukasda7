//----------------------------------------------------------------------------------------
/**
 * \file       renderableObject.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Renderable object class declaration.
 *
 *  Declares drawable object data and interfaces for mesh, material and transform-driven rendering.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include "object.h"

/**
 * \brief      Renderable object class declaration.
 */
typedef struct _ShaderProgram {
    GLuint program;       ///< Shader program identifier.

    bool initialized;     ///< Whether required shader locations are loaded.

    /**
      * \brief Indices of the vertex shader inputs (locations).
      */
    struct {
        // vertex attributes locations
        GLint position;
        GLint normal;
        GLint texCoord;

        // uniform matrices locations
        GLint projectionMatrix;
        GLint viewMatrix;
        GLint modelMatrix;
        GLint normalMatrix;
        GLint pvmMatrix;

        // uniform material locations
        GLint diffuse;
        GLint specular;
        GLint ambient;
        GLint shininess;

        // uniform fog locations
        GLint fogCenter;
        GLint fogCenter2;
        GLint fogColor;
        GLint fogColor2;
        GLint fogRadius;
        GLint fogRadius2;
        GLint fogDensity;
        GLint fogDensity2;

        // uniform misc locations
        GLint elapsedTime;
        GLint ambientColor;
        GLint cameraPosition;
        GLint asteroidLocation;
        GLint hasDiffuseTexture;
        GLint hasNormalTexture;
        GLint hasSpecularTexture;
        GLint isUVAnimated;
        GLint isDisplaceAnimated;
        GLint displacementSize;
        
        // uniform mandelbrot locations
        GLint mandelbrotAnimStarted;
        GLint mandelbrotAnimPaused;
        GLint mandelbrotAnimStartTime;
        GLint mandelbrotAnimPauseTime;
        GLint mandelbrotMaxIterations;
        GLint mandelbrotZoomSpeed;
        GLint mandelbrotColorSpeed;
        GLint mandelbrotZoomTarget;
    } locations;

    /// \brief Creates an empty structure with all locations set to -1.
    _ShaderProgram() : program(0), initialized(false) {
        locations.position = -1;
        locations.normal = -1;
        locations.texCoord = -1;

        locations.projectionMatrix = -1;
        locations.viewMatrix = -1;
        locations.modelMatrix = -1;
        locations.normalMatrix = -1;
        locations.pvmMatrix = -1;

        locations.diffuse = -1;
        locations.specular = -1;
        locations.ambient = -1;
        locations.shininess = -1;

        locations.fogCenter = -1;
        locations.fogCenter2 = -1;
        locations.fogColor = -1;
        locations.fogColor2 = -1;
        locations.fogRadius = -1;
        locations.fogRadius2 = -1;
        locations.fogDensity = -1;
        locations.fogDensity2 = -1;

        locations.elapsedTime = -1;
        locations.ambientColor = -1;
        locations.cameraPosition = -1;
        locations.asteroidLocation = -1;
        locations.hasDiffuseTexture = -1;
        locations.hasNormalTexture = -1;
        locations.hasSpecularTexture = -1;
        locations.isUVAnimated = -1;
        locations.isDisplaceAnimated = -1;
        locations.displacementSize = -1;

        locations.mandelbrotAnimPaused = -1;
        locations.mandelbrotAnimStarted = -1;
        locations.mandelbrotAnimStartTime = -1;
        locations.mandelbrotAnimPauseTime = -1;
        locations.mandelbrotMaxIterations = -1;
        locations.mandelbrotZoomSpeed = -1;
        locations.mandelbrotColorSpeed = -1;
        locations.mandelbrotZoomTarget = -1;
    }

} ShaderProgram;

/**
 * \brief      Renderable object class declaration.
 */
typedef struct _ObjectGeometry {
    GLuint        vertexBufferObject;    ///< Identifier for the vertex buffer object.
    GLuint        normalBufferObject;    ///< Identifier for the normal buffer object.
    GLuint        elementBufferObject;   ///< Identifier for the element buffer object.
    GLuint        texCoordBufferObject;  ///< Identifier for the texture coordinate buffer object.
    GLuint        vertexArrayObject;     ///< Identifier for the vertex array object.
    GLuint        diffuseTextureObject;  ///< Identifier for the diffuse texture object.
    GLuint        normalTextureObject;   ///< Identifier for the normal texture object.
    GLuint        specularTextureObject; ///< Identifier for the specular texture object.
    bool          hasTexture;            ///< Whether has diffuse texture coordinates and texture assigned.
    bool          hasNormalTexture;      ///< Whether has normal texture coordinates and texture assigned.
    bool          hasSpecularTexture;    ///< Whether has specular texture coordinates and texture assigned.
    unsigned int  numTriangles;          ///< Number of triangles in the mesh.
} ObjectGeometry;

/**
 * \brief      Renderable object class declaration.
 */
typedef struct _Material {
    glm::vec3 ambient;      ///< Material ambient component.
    glm::vec3 diffuse;      ///< Material diffuse component.
    glm::vec3 specular;     ///< Material specular component.
    float     shininess;    ///< Material specular highlight sharpness.
} Material;

/**
 * \brief      Renderable object class declaration.
 */
class RenderableObject : public Object {
protected:
    ObjectGeometry* geometry;      ///< Object geometry uploaded to GPU.
    ShaderProgram* shaderProgram;  ///< Shader program used for rendering.
    Material material;             ///< Object material parameters.
    bool hasMaterial;              ///< Whether material was explicitly assigned.

    /// \brief Uploads material uniforms to active shader.
    void applyMaterialUniforms() const {
        if ((shaderProgram == nullptr) || !hasMaterial)
            return;

        if (shaderProgram->locations.diffuse != -1)
            glUniform3fv(shaderProgram->locations.diffuse, 1, glm::value_ptr(material.diffuse));

        if (shaderProgram->locations.specular != -1)
            glUniform3fv(shaderProgram->locations.specular, 1, glm::value_ptr(material.specular));

        if (shaderProgram->locations.ambient != -1)
            glUniform3fv(shaderProgram->locations.ambient, 1, glm::value_ptr(material.ambient));

        if (shaderProgram->locations.shininess != -1)
            glUniform1f(shaderProgram->locations.shininess, material.shininess);
    }

public:
    /**
     * \brief Creates a renderable object.
     * \param shdrPrg Shader program used for drawing.
     * \param mat Optional material settings.
     */
    explicit RenderableObject(ShaderProgram* shdrPrg = nullptr,
                              const Material* mat = nullptr)
        : Object()
        , geometry(nullptr)
        , shaderProgram(shdrPrg)
        , material{glm::vec3(0.0f), glm::vec3(0.8f), glm::vec3(0.0f), 1.0f}
        , hasMaterial(mat != nullptr) {
        if (mat != nullptr)
            material = *mat;
    }
    /// \brief Virtual destructor.
    ~RenderableObject() override = default;
};
