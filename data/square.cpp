#include <iostream>
#include "square.h"

const float Square::vertices[30] = {
    //  X   Y   Z   U   V
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

Square::Square(ShaderProgram* shdrPrg)
               : RenderableObject(shdrPrg),
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

void Square::update(float elapsedTime, const glm::mat4* parentModelMatrix) {
    // instance specific stuff

    // propagate the update to children
    RenderableObject::update(elapsedTime, parentModelMatrix);
}

void Square::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    if (initialized && (shaderProgram != nullptr)) {
        glUseProgram(shaderProgram->program);

        const glm::mat4 pvmMatrix = projectionMatrix * viewMatrix * globalModelMatrix;
        glUniformMatrix4fv(shaderProgram->locations.PVMmatrix,
                           1, GL_FALSE, glm::value_ptr(pvmMatrix));


        glBindVertexArray(geometry->vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, geometry->numTriangles*3);
        glBindVertexArray(0);
    }

    RenderableObject::draw(viewMatrix, projectionMatrix);
}

void Square::initializeSquare() {
    geometry = new ObjectGeometry;

    geometry->numTriangles = 2;
    geometry->elementBufferObject = 0;

    glGenVertexArrays(1, &(geometry->vertexArrayObject));
    glBindVertexArray(geometry->vertexArrayObject);

    glGenBuffers(1, &geometry->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    if ((shaderProgram != nullptr) &&
        shaderProgram->initialized &&
        (shaderProgram->locations.position != -1) &&
        (shaderProgram->locations.PVMmatrix != -1)) {

        //enabling vertex position attribute
        glEnableVertexAttribArray(shaderProgram->locations.position);
        glVertexAttribPointer(
            shaderProgram->locations.position, 3, GL_FLOAT,
            GL_FALSE, 5 * sizeof(float), (void*)0
        );

        //enabling vertex texture coord attribute
        glEnableVertexAttribArray(shaderProgram->locations.texCoord);
        glVertexAttribPointer(
            shaderProgram->locations.texCoord, 2, GL_FLOAT,
            GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))
        );

        initialized = true;
    }

    //REMOVE
    localModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

    glBindVertexArray(0);
}

