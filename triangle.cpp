#include <iostream>
#include "triangle.h"

void Triangle::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (initialized && (shaderProgram != nullptr)) {
        glUseProgram(shaderProgram->program);
        applyMaterialUniforms();

        glUniformMatrix4fv(shaderProgram->locations.projectionMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(geometry->vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
    else {
        std::cerr << "Triangle::draw(): Can't draw, triangle not initialized properly!" << std::endl;
    }

    Object::draw(viewMatrix, projectionMatrix);
}

Triangle::Triangle(ShaderProgram* shdrPrg) : RenderableObject(shdrPrg), initialized(false)
{
    geometry = new ObjectGeometry;

    static constexpr float vertices[] = {
      0.0f,  0.5f,
     -0.5f, -0.5f,
      0.5f, -0.5f,
    };

    geometry->numTriangles = 1;
    geometry->elementBufferObject = 0;
    geometry->normalBufferObject = 0;
    geometry->texCoordBufferObject = 0;
    geometry->diffuseTextureObject = 0;
    geometry->normalTextureObject = 0;
    geometry->specularTextureObject = 0;
    geometry->hasTexture = false;
    geometry->hasNormalTexture = false;
    geometry->hasSpecularTexture = false;

    glGenVertexArrays(1, &geometry->vertexArrayObject);
    glBindVertexArray(geometry->vertexArrayObject);

    glGenBuffers(1, &geometry->vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    if ((shaderProgram != nullptr) && shaderProgram->initialized && (shaderProgram->locations.position != -1)) {
        glEnableVertexAttribArray(shaderProgram->locations.position);
        glVertexAttribPointer(shaderProgram->locations.position, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        initialized = true;
    }
    else {
        std::cerr << "Triangle::Triangle(): shaderProgram struct not initialized!" << std::endl;
    }
}

Triangle::~Triangle() {
    glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
    glDeleteBuffers(1, &(geometry->elementBufferObject));
    glDeleteBuffers(1, &(geometry->vertexBufferObject));

    delete geometry;
    geometry = nullptr;

    initialized = false;
}
