//----------------------------------------------------------------------------------------
/**
 * \file       light.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Light source class declaration.
 *
 *  Declares light properties and interfaces for point, spot and directional lighting in the scene.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include "object.h"

/**
 * \brief      Light source class declaration.
 */
enum LightType { POINT_LIGHT, SPOT_LIGHT, DIRECTION_LIGHT };

/**
 * \brief      Light source class declaration.
 */
class Light : public Object {
public:
    /**
     * \brief Creates a light with parameters shared across all shader paths.
     * \param lightType Light type.
     * \param ambient Ambient component.
     * \param diffuse Diffuse component.
     * \param specular Specular component.
     * \param spotDirection Spot cone direction.
     * \param spotCutOff Spot cutoff angle in cosine form.
     * \param spotExponent Spot cone concentration exponent.
     * \param intensity Light intensity.
     */
    explicit Light(
        LightType lightType,
        glm::vec3 ambient = glm::vec3(0.0f),
        glm::vec3 diffuse = glm::vec3(1.0f),
        glm::vec3 specular = glm::vec3(1.0f),
        glm::vec3 spotDirection = glm::vec3(0.0f, 0.0f, -1.0f),
        float spotCutOff = cos(glm::radians(45.0f)),
        float spotExponent = 2.0f,
        float intensity = 1.0f
    );
    /// \brief Virtual destructor.
    ~Light() override = default;

    /// \brief Returns light type.
    LightType getLightType() const;
    /// \brief Returns ambient component.
    const glm::vec3& getAmbient() const;
    /// \brief Returns diffuse component.
    const glm::vec3& getDiffuse() const;
    /// \brief Returns specular component.
    const glm::vec3& getSpecular() const;
    /// \brief Returns spot cone direction.
    const glm::vec3& getSpotDirection() const;
    /// \brief Returns spot cone cutoff.
    float getSpotCutOff() const;
    /// \brief Returns spot cone exponent.
    float getSpotExponent() const;
    /// \brief Returns light intensity.
    float getIntensity() const;

    /// \brief Sets light type.
    void setLightType(LightType lightType);
    /// \brief Sets ambient component.
    void setAmbient(glm::vec3 ambient);
    /// \brief Sets diffuse component.
    void setDiffuse(glm::vec3 diffuse);
    /// \brief Sets specular component.
    void setSpecular(glm::vec3 specular);
    /// \brief Sets spot cone direction.
    void setSpotDirection(glm::vec3 direction);
    /// \brief Sets spot cone cutoff.
    void setSpotCutOff(float cutoff);
    /// \brief Sets spot cone exponent.
    void setSpotExponent(float exponent);
    /// \brief Sets light intensity.
    void setIntensity(float intensity);

private:
    LightType lightType;        ///< Light type.
    glm::vec3 ambient;          ///< Ambient component.
    glm::vec3 diffuse;          ///< Diffuse component.
    glm::vec3 specular;         ///< Specular component.
    glm::vec3 spotDirection;    ///< Spot cone direction.
    float spotCutOff;           ///< Spot cone cutoff in cosine form.
    float spotExponent;         ///< Spot cone concentration exponent.
    float intensity;            ///< Light intensity.
};
