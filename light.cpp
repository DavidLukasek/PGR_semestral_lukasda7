#include "light.h"

Light::Light(
    LightType lightType,
    glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 spotDirection,
    glm::vec3 spotCutOff,
    float spotExponent
)
    : Object()
    , lightType(lightType)
    , ambient(ambient)
    , diffuse(diffuse)
    , specular(specular)
    , spotDirection(spotDirection)
    , spotCutOff(spotCutOff)
    , spotExponent(spotExponent) {
}

LightType Light::getLightType() const {
    return lightType;
}

const glm::vec3& Light::getAmbient() const {
    return ambient;
}

const glm::vec3& Light::getDiffuse() const {
    return diffuse;
}

const glm::vec3& Light::getSpecular() const {
    return specular;
}

const glm::vec3& Light::getSpotDirection() const {
    return spotDirection;
}

const glm::vec3& Light::getSpotCutOff() const {
    return spotCutOff;
}

float Light::getSpotExponent() const {
    return spotExponent;
}
