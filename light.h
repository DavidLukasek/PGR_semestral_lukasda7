#pragma once

#include "object.h"

enum LightType { POINT_LIGHT, SPOT_LIGHT, DIRECTION_LIGHT };

class Light : public Object {
public:
    explicit Light(
        LightType lightType,
        glm::vec3 ambient = glm::vec3(0.0f),
        glm::vec3 diffuse = glm::vec3(1.0f),
        glm::vec3 specular = glm::vec3(1.0f),
        glm::vec3 spotDirection = glm::vec3(0.0f, 0.0f, -1.0f),
        float spotCutOff = 1.0f,
        float spotExponent = 1.0f
    );
    ~Light() override = default;

    LightType getLightType() const;
    const glm::vec3& getAmbient() const;
    const glm::vec3& getDiffuse() const;
    const glm::vec3& getSpecular() const;
    const glm::vec3& getSpotDirection() const;
    float getSpotCutOff() const;
    float getSpotExponent() const;

private:
    LightType lightType;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 spotDirection;
    float spotCutOff;
    float spotExponent;
};
