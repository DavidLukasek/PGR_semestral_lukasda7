#include <iostream>
#include "square.h"

const float Square::vertices[30] = {
    // X     Y     Z     U     V
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

Square::Square(ShaderProgram* shdrPrg)
               : SingleMesh("square", shdrPrg, nullptr),
               initialized(false) {
    initializeSquare();
}

Square::~Square() {
    glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
    glDeleteBuffers(1, &(geometry->elementBufferObject));
    glDeleteBuffers(1, &(geometry->vertexBufferObject));

    delete geometry;
    geometry = nullptr;

    initialized = false;
}

void Square::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (initialized && (shaderProgram != nullptr)) {
        glUseProgram(shaderProgram->program);
        applyMaterialUniforms();

        if (shaderProgram->locations.projectionMatrix != -1) {
            glUniformMatrix4fv(shaderProgram->locations.projectionMatrix,
                               1, GL_FALSE, glm::value_ptr(projectionMatrix));
        }

        if (shaderProgram->locations.viewMatrix != -1) {
            glUniformMatrix4fv(shaderProgram->locations.viewMatrix,
                               1, GL_FALSE, glm::value_ptr(viewMatrix));
        }

        if (shaderProgram->locations.modelMatrix != -1) {
            glUniformMatrix4fv(shaderProgram->locations.modelMatrix,
                               1, GL_FALSE, glm::value_ptr(globalModelMatrix));
        }

        if (shaderProgram->locations.pvmMatrix != -1) {
            const glm::mat4 pvmMatrix = projectionMatrix * viewMatrix * globalModelMatrix;
            glUniformMatrix4fv(shaderProgram->locations.pvmMatrix,
                               1, GL_FALSE, glm::value_ptr(pvmMatrix));
        }

        glBindVertexArray(geometry->vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, geometry->numTriangles * 3);
        glBindVertexArray(0);
    }

    Object::draw(viewMatrix, projectionMatrix);
}

void Square::initializeSquare() {
    geometry = new ObjectGeometry;

    geometry->numTriangles = 2;
    geometry->elementBufferObject = 0;
    geometry->normalBufferObject = 0;
    geometry->texCoordBufferObject = 0;
    geometry->diffuseTextureObject = 0;
    geometry->normalTextureObject = 0;
    geometry->specularTextureObject = 0;
    geometry->hasTexture = false;
    geometry->hasNormalTexture = false;
    geometry->hasSpecularTexture = false;

    glGenVertexArrays(1, &(geometry->vertexArrayObject));
    glBindVertexArray(geometry->vertexArrayObject);

    glGenBuffers(1, &geometry->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    if ((shaderProgram != nullptr) &&
        shaderProgram->initialized &&
        (shaderProgram->locations.position != -1)) {

        // enabling vertex position attribute
        glEnableVertexAttribArray(shaderProgram->locations.position);
        glVertexAttribPointer(
            shaderProgram->locations.position, 3, GL_FLOAT,
            GL_FALSE, 5 * sizeof(float), (void*)0
        );

        // enabling vertex texture coord attribute
        if (shaderProgram->locations.texCoord != -1) {
            glEnableVertexAttribArray(shaderProgram->locations.texCoord);
            glVertexAttribPointer(
                shaderProgram->locations.texCoord, 2, GL_FLOAT,
                GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))
            );
        }

        initialized = true;
    }

    setLocalModelMatrix(
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f))
    );

    glBindVertexArray(0);
}
