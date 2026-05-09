//----------------------------------------------------------------------------------------
/**
 * \file       light.cpp
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Light source class implementation.
 *
 *  Implements light data handling and helper logic used during shading and scene setup.
 *
*/
//----------------------------------------------------------------------------------------
#include "light.h"

Light::Light(
    LightType lightType,
    glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 spotDirection,
    float spotCutOff,
    float spotExponent,
    float intensity
)
    : Object()
    , lightType(lightType)
    , ambient(ambient)
    , diffuse(diffuse)
    , specular(specular)
    , spotDirection(spotDirection)
    , spotCutOff(spotCutOff)
    , spotExponent(spotExponent)
    , intensity(intensity) {
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

float Light::getSpotCutOff() const {
    return spotCutOff;
}

float Light::getSpotExponent() const {
    return spotExponent;
}

float Light::getIntensity() const {
    return intensity;
}

void Light::setLightType(LightType lightType) {
    this->lightType = lightType;
}

void Light::setAmbient(glm::vec3 ambient) {
    this->ambient = ambient;
}

void Light::setDiffuse(glm::vec3 diffuse) {
    this->diffuse = diffuse;
}

void Light::setSpecular(glm::vec3 specular) {
    this->specular = specular;
}

void Light::setSpotDirection(glm::vec3 direction) {
    this->spotDirection = direction;
}

void Light::setSpotCutOff(float cutoff) {
    this->spotCutOff = cutoff;
}

void Light::setSpotExponent(float exponent) {
    this->spotExponent = exponent;
}

void Light::setIntensity(float intensity) {
    this->intensity = intensity;
}
