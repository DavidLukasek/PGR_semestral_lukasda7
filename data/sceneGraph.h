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
    glm::vec3(1.0f),                // diffuse
    glm::vec3(1.0f),                // specular
    10.0f                           // shininess
};

Material material2 = {
    glm::vec3(0.0f),                // ambient
    glm::vec3(1.0f),                // diffuse
    glm::vec3(0.23f, 0.20f, 0.11f), // specular
    10.0f                           // shininess
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
                                              glm::vec3(0.0f, -1.0f, 2.0f)));
    objects.push_back(floor);

    // Susanne
    SingleMesh* monke = new SingleMesh(MODELS_PATH + (std::string)"monke.obj",
                                       &phongShaderProgram,
                                       &material2);
    monke->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                              glm::vec3(-2.0f, 0.0f, 2.0f)) *
                               glm::rotate(glm::mat4(1.0f),
                                           glm::radians(90.0f),
                                           glm::vec3(0.0f, 1.0f, 0.0f)));
    objects.push_back(monke);

    // point light
    Light* pointLight1 = new Light(POINT_LIGHT,         // light type
                                   glm::vec3(0.0f),     // ambient
                                   glm::vec3(1.0f),     // diffuse
                                   glm::vec3(1.0f));    // specular
    pointLight1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                     glm::vec3(-1.5f, 0.0f, 3.5f)));
    objects.push_back(pointLight1);

    // spot light 1
    Light* spotLight1 = new Light(SPOT_LIGHT,                   // light type
                                  glm::vec3(0.0f),              // ambient
                                  glm::vec3(1.0f, 0.0f, 0.0f),  // diffuse
                                  glm::vec3(1.0f, 0.0f, 0.0f),  // specular
                                  glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                  cos(glm::radians(45.0f)),     // spot cutoff
                                  5.0f);                        // spot exponent
    spotLight1->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f)));
    objects.push_back(spotLight1);

    // spot light 2
    Light* spotLight2 = new Light(SPOT_LIGHT,                   // light type
                                  glm::vec3(0.0f),              // ambient
                                  glm::vec3(0.0f, 1.0f, 0.0f),  // diffuse
                                  glm::vec3(0.0f, 1.0f, 0.0f),  // specular
                                  glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                  cos(glm::radians(45.0f)),     // spot cutoff
                                  5.0f);                        // spot exponent
    spotLight2->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                    glm::vec3(0.5f, 0.0f, 1.5f)));
    objects.push_back(spotLight2);

    // spot light 3
    Light* spotLight3 = new Light(SPOT_LIGHT,                   // light type
                                  glm::vec3(0.0f),              // ambient
                                  glm::vec3(0.0f, 0.0f, 1.0f),  // diffuse
                                  glm::vec3(0.0f, 0.0f, 1.0f),  // specular
                                  glm::vec3(0.0f, -1.0f, 0.0f), // spot direction
                                  cos(glm::radians(45.0f)),     // spot cutoff
                                  1.0f);                        // spot exponent
    spotLight3->setLocalModelMatrix(glm::translate(glm::mat4(1.0f),
                                    glm::vec3(-0.5f, 0.0f, 1.5f)));
    objects.push_back(spotLight3);
}
