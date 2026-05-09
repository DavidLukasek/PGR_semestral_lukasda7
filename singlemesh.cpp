//----------------------------------------------------------------------------------------
/**
 * \file       singlemesh.cpp
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Single mesh wrapper implementation.
 *
 *  Implements loading, storage and draw helper logic for a single mesh instance.
 *
*/
//----------------------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include "singlemesh.h"

SingleMesh::SingleMesh(std::string modelFileName,
    ShaderProgram* shdrPrg,
    const Material* mat)
    : RenderableObject(shdrPrg, mat)
    , initialized(false)
    , backFaceCullingOff(false)
    , isUVAnimated(false)
    , isDisplaceAnimated(false)
    , additiveBlending(false) {
    if (modelFileName != "square") {
        if (!loadSingleMesh(modelFileName, shdrPrg, &geometry)) {
            if (geometry == nullptr) {
                std::cerr << "SingleMesh::SingleMesh(): geometry not initialized!" << std::endl;
            }
            else {
                std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
            }
        }
    }
    
    if ((shaderProgram != nullptr) && shaderProgram->initialized) {
            initialized = true;
    } else {
            std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
    }
}

SingleMesh::~SingleMesh() {

    if (geometry != nullptr) {
        glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
        glDeleteBuffers(1, &(geometry->normalBufferObject));
        glDeleteBuffers(1, &(geometry->texCoordBufferObject));
        glDeleteBuffers(1, &(geometry->elementBufferObject));
        glDeleteBuffers(1, &(geometry->vertexBufferObject));
        glDeleteTextures(1, &(geometry->diffuseTextureObject));
        glDeleteTextures(1, &(geometry->normalTextureObject));
        glDeleteTextures(1, &(geometry->specularTextureObject));

        delete geometry;
        geometry = nullptr;
    }

    initialized = false;
}

void SingleMesh::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (!isVisible())
        return;

    // updating uniforms
    if (initialized && (shaderProgram != nullptr)) {
        // material uniforms
        glUseProgram(shaderProgram->program);
        applyMaterialUniforms();

        // matrix uniforms
        if (shaderProgram->locations.projectionMatrix != -1)
            glUniformMatrix4fv(shaderProgram->locations.projectionMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        
        if (shaderProgram->locations.viewMatrix != -1)
            glUniformMatrix4fv(shaderProgram->locations.viewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        if (shaderProgram->locations.modelMatrix != -1)
            glUniformMatrix4fv(shaderProgram->locations.modelMatrix, 1, GL_FALSE, glm::value_ptr(globalModelMatrix));

        if (shaderProgram->locations.normalMatrix != -1) {
            const glm::mat4 normalMatrix = glm::transpose(glm::inverse(globalModelMatrix));
            glUniformMatrix4fv(shaderProgram->locations.normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        if (shaderProgram->locations.pvmMatrix != -1) {
            glm::mat4 pvmMatrix;
            // boolean for rocketflame shader to exclude modelmatrix from PVM
            const bool isRocketFlameLikeShader =
                (shaderProgram->locations.modelMatrix != -1) &&
                (shaderProgram->locations.normalMatrix == -1) &&
                (shaderProgram->locations.cameraPosition != -1) &&
                (shaderProgram->locations.elapsedTime != -1) &&
                (shaderProgram->locations.isUVAnimated == -1);

            if (isRocketFlameLikeShader)
                pvmMatrix = projectionMatrix * viewMatrix;
            else if (shaderProgram->locations.modelMatrix != -1)
                pvmMatrix = projectionMatrix * viewMatrix * globalModelMatrix;
            else
                pvmMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));

            glUniformMatrix4fv(shaderProgram->locations.pvmMatrix, 1, GL_FALSE, glm::value_ptr(pvmMatrix));
        }

        // texture flag uniforms
        const bool hasDiffuseTexture =
            (geometry != nullptr) && geometry->hasTexture && (geometry->diffuseTextureObject != 0);
        const bool hasNormalTexture =
            (geometry != nullptr) && geometry->hasNormalTexture && (geometry->normalTextureObject != 0);
        const bool hasSpecularTexture =
            (geometry != nullptr) && geometry->hasSpecularTexture && (geometry->specularTextureObject != 0);

        if (shaderProgram->locations.hasDiffuseTexture != -1)
            glUniform1i(shaderProgram->locations.hasDiffuseTexture, hasDiffuseTexture ? 1 : 0);
        if (shaderProgram->locations.hasNormalTexture != -1)
            glUniform1i(shaderProgram->locations.hasNormalTexture, hasNormalTexture ? 1 : 0);
        if (shaderProgram->locations.hasSpecularTexture != -1)
            glUniform1i(shaderProgram->locations.hasSpecularTexture, hasSpecularTexture ? 1 : 0);

        auto setSamplerUnitIfPresent = [&](const char* uniformName, GLint unit) {
            const GLint location = glGetUniformLocation(shaderProgram->program, uniformName);
            if (location != -1)
                glUniform1i(location, unit);
        };

        // diffuse texture uniforms
        if (hasDiffuseTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, geometry->diffuseTextureObject);

            // Support both naming conventions used in this project.
            setSamplerUnitIfPresent("diffuseTex", 0);
            setSamplerUnitIfPresent("diffuseSampler", 0);
        }
        else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // normal texture uniforms
        if (hasNormalTexture) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, geometry->normalTextureObject);

            // Support both naming conventions used in this project.
            setSamplerUnitIfPresent("normalTex", 1);
            setSamplerUnitIfPresent("normalSampler", 1);
        }
        else {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // specular texture uniforms
        if (hasSpecularTexture) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, geometry->specularTextureObject);

            // support both naming conventions used in this project
            setSamplerUnitIfPresent("specularTex", 2);
            setSamplerUnitIfPresent("specularSampler", 2);
        }
        else {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // turning off backface culling for objects that have it off
        if (backFaceCullingOff) {
            glDisable(GL_CULL_FACE);
            glDepthMask(GL_FALSE);
        }

        // additive blending is useful for emissive/translucent effects (e.g. flames)
        if (additiveBlending) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
        // turning on UV animation if enabled
        if (shaderProgram->locations.isUVAnimated != -1) {
            glUniform1i(shaderProgram->locations.isUVAnimated, isUVAnimated ? 1 : 0);
        }
        // turning on displacement animation if enabled
        if (shaderProgram->locations.isDisplaceAnimated != -1) {
            glUniform1i(shaderProgram->locations.isDisplaceAnimated,
                        isDisplaceAnimated ? 1 : 0);
        }

        // drawing
        glBindVertexArray(geometry->vertexArrayObject);
        glDrawElements(GL_TRIANGLES, geometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (hasDiffuseTexture)
            glBindTexture(GL_TEXTURE_2D, 0);
        if (hasNormalTexture) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        if (hasSpecularTexture) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glActiveTexture(GL_TEXTURE0);

        if (additiveBlending) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        // turning all on again if turned off due to being transparent clouds
        if (backFaceCullingOff) {
            glEnable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);
        }
    }
    else {
        std::cerr << "SingleMesh::draw(): Can't draw, mesh not initialized properly!" << std::endl;
    }

    // draw children
    Object::draw(viewMatrix, projectionMatrix);
}

bool SingleMesh::getDisplaceAnimated() {
    return isDisplaceAnimated;
}

void SingleMesh::setBackfaceCullingOff(bool value) {
    backFaceCullingOff = value;
}

void SingleMesh::setUVAnimated(bool value) {
    isUVAnimated = value;
}

void SingleMesh::setDisplaceAnimated(bool value) {
    isDisplaceAnimated = value;
}

void SingleMesh::setAdditiveBlending(bool value) {
    additiveBlending = value;
}

/**
 * \brief      Single mesh wrapper implementation.
 *        Vertices-only variant for this project.
 * \param fileName [in] File to open and load.
 * \param shader [in] Shader used to connect loaded data to VAO attributes.
 * \param geometry [out] Output mesh geometry structure.
 */
bool SingleMesh::loadSingleMesh(const std::string& fileName, ShaderProgram* shader, ObjectGeometry** geometry) {
    Assimp::Importer importer;

    // unitize object in size (scale the model to fit into (-1..1)^3)
    // importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

    // load asset from the file - you can play with various processing steps
    const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
        | aiProcess_Triangulate             // triangulate polygons (if any)
        | aiProcess_PreTransformVertices    // transforms scene hierarchy into one root with geometry-leafs only, for more see Doc
        | aiProcess_GenSmoothNormals        // calculate normals per vertex
        | aiProcess_JoinIdenticalVertices);

    // abort if the loader fails
    if (scn == NULL) {
        std::cerr << "SingleMesh::loadSingleMesh(): assimp error - " << importer.GetErrorString() << std::endl;
        *geometry = NULL;
        return false;
    }

    // some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
    if (scn->mNumMeshes != 1) {
        std::cerr << "SingleMesh::loadSingleMesh(): this simplified loader can only process files with only one mesh" << std::endl;
        *geometry = NULL;
        return false;
    }

    // in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
    const aiMesh* mesh = scn->mMeshes[0];

    *geometry = new ObjectGeometry;
    (*geometry)->texCoordBufferObject = 0;
    (*geometry)->diffuseTextureObject = 0;
    (*geometry)->normalTextureObject = 0;
    (*geometry)->specularTextureObject = 0;
    (*geometry)->hasTexture = false;
    (*geometry)->hasNormalTexture = false;
    (*geometry)->hasSpecularTexture = false;

    // vertex buffer object, store all vertex positions
    glGenBuffers(1, &((*geometry)->vertexBufferObject));
    glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW);     // allocate memory for vertices
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices); // store all vertices

    // normal buffer object, store all normals
    glGenBuffers(1, &((*geometry)->normalBufferObject));
    glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->normalBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);

    // texture coordinate buffer object, store all UVs (if available)
    if (mesh->HasTextureCoords(0)) {
        std::vector<float> texCoords;
        texCoords.reserve(mesh->mNumVertices * 2);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            texCoords.push_back(mesh->mTextureCoords[0][i].x);
            texCoords.push_back(mesh->mTextureCoords[0][i].y);
        }

        glGenBuffers(1, &((*geometry)->texCoordBufferObject));
        glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->texCoordBufferObject);
        glBufferData(GL_ARRAY_BUFFER,
                     2 * sizeof(float) * mesh->mNumVertices,
                     texCoords.data(),
                     GL_STATIC_DRAW);
    }

    // copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
    unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
    for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
        indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
        indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
        indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
    }

    // copy our temporary index array to OpenGL and free the array
    glGenBuffers(1, &((*geometry)->elementBufferObject));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

    delete[] indices;

    // copy the material info to MeshGeometry structure
    const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
    aiColor4D color;
    aiString name;
    aiReturn retValue = AI_SUCCESS;

    // get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
    mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing, must be aiString type!

    if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
        color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

    // lambda function for resolving texture paths to absolute
    auto resolveTexturePath = [&](const aiString& texturePath) -> std::string {
        std::string resolvedTexturePath = texturePath.C_Str();
        const bool isAbsolutePath = (resolvedTexturePath.size() > 1 && resolvedTexturePath[1] == ':')
                                    || (!resolvedTexturePath.empty() && (resolvedTexturePath[0] == '/' || resolvedTexturePath[0] == '\\'));

        if (!isAbsolutePath) {
            const size_t lastSlash = fileName.find_last_of("/\\");
            const std::string modelDir = (lastSlash == std::string::npos) ? "" : fileName.substr(0, lastSlash + 1);
            resolvedTexturePath = modelDir + resolvedTexturePath;
        }

        return resolvedTexturePath;
    };

    // lambda function for loading textures
    auto loadTexture = [&](aiTextureType type, const char* textureRole, GLuint& textureObject) -> bool {
        aiString texturePath;
        if (mat->GetTexture(type, 0, &texturePath) != AI_SUCCESS)
            return false;

        const std::string resolvedTexturePath = resolveTexturePath(texturePath);
        textureObject = pgr::createTexture(resolvedTexturePath.c_str());
        return textureObject != 0;
    };

    // load diffuse map and check if it succeeded
    if (loadTexture(aiTextureType_DIFFUSE, "diffuse", (*geometry)->diffuseTextureObject)) {
        (*geometry)->hasTexture = mesh->HasTextureCoords(0);
    }

    // load normal map and check if it succeeded
    const bool hasLoadedNormalMap =
        loadTexture(aiTextureType_NORMALS, "normal (NORMALS)", (*geometry)->normalTextureObject) ||
        loadTexture(aiTextureType_HEIGHT, "normal (HEIGHT/map_Bump)", (*geometry)->normalTextureObject);
    if (hasLoadedNormalMap)
        (*geometry)->hasNormalTexture = mesh->HasTextureCoords(0);

    // load specular map and check if it succeeded
    const bool hasLoadedSpecularMap =
        loadTexture(aiTextureType_SPECULAR, "specular (SPECULAR)", (*geometry)->specularTextureObject) ||
        loadTexture(aiTextureType_SHININESS, "specular (SHININESS/map_Ns)", (*geometry)->specularTextureObject);
    if (hasLoadedSpecularMap) {
        (*geometry)->hasSpecularTexture = mesh->HasTextureCoords(0);
    }

    glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
    glBindVertexArray((*geometry)->vertexArrayObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);

    bool validInit = false;

    if ((shader != nullptr) && shader->initialized) {
        // position
        if (shader->locations.position != -1) {
            glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
            glEnableVertexAttribArray(shader->locations.position);
            glVertexAttribPointer(shader->locations.position, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

        // normal
        if (shader->locations.normal != -1) {
            glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->normalBufferObject);
            glEnableVertexAttribArray(shader->locations.normal);
            glVertexAttribPointer(shader->locations.normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

        // texture coordinates
        if ((shader->locations.texCoord != -1) && mesh->HasTextureCoords(0) && ((*geometry)->texCoordBufferObject != 0)) {
            glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->texCoordBufferObject);
            glEnableVertexAttribArray(shader->locations.texCoord);
            glVertexAttribPointer(shader->locations.texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }

        CHECK_GL_ERROR();
        validInit = true;
    }

    glBindVertexArray(0);

    (*geometry)->numTriangles = mesh->mNumFaces;

    return validInit;
}
