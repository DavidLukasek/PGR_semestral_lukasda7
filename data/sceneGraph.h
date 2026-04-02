#pragma once

#include <vector>

#include "../singlemesh.h"
#include "models/square.h"

ShaderProgram commonShaderProgram;
ShaderProgram mandelrotShaderProgram;
ShaderProgram phongShaderProgram;

ObjectList objects;

Material material1 = {
    glm::vec3(0.0f),                // ambient
    glm::vec3(0.76f, 0.60f, 0.38f), // diffuse
    glm::vec3(0.18f, 0.14f, 0.08f), // specular
    16.0f                           // shininess
};

Material material2 = {
    glm::vec3(0.0f),                // ambient
    glm::vec3(0.76f, 0.60f, 0.38f), // diffuse
    glm::vec3(0.18f, 0.14f, 0.08f), // specular
    16.0f                           // shininess
};

Camera camera;
glm::vec2 cameraRotation = glm::vec2(0.0);

void createObjects() {
    // 3 triangles shape
    objects.push_back(new SingleMesh(MODELS_PATH + (std::string)"shape.obj",
                                     &commonShaderProgram));
    // square with mandelbrot set
    objects.push_back(new Square(&mandelrotShaderProgram));

    // floor mesh
    SingleMesh* floor = new SingleMesh(MODELS_PATH + (std::string)"floor.obj",
                                       &phongShaderProgram,
                                       &material1);
    floor->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              glm::vec3(0.0f, -1.0f, 0.0f)));
    objects.push_back(floor);

    // monkey shape
    SingleMesh* monke = new SingleMesh(MODELS_PATH + (std::string)"monke.obj",
                                       &phongShaderProgram,
                                       &material1);
    monke->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              glm::vec3(-2.0f, 0.0f, 1.0f)) *
                               glm::rotate(glm::mat4(1.0f),
                                           glm::radians(90.0f),
                                           glm::vec3(0.0f, 1.0f, 0.0f)));
    objects.push_back(monke);

    // point light
    Light* pointLight1 = new Light(POINT_LIGHT,          // light type
                                   glm::vec3(0.0f),     // ambient
                                   glm::vec3(1.0f),     // diffuse
                                   glm::vec3(1.0f),     // specular
                                   glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                   1.0f,                // spot cutoff
                                   1.0f);               // spot exponent
    pointLight1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0f, 0.0f, 1.0f)));
    objects.push_back(pointLight1);
}