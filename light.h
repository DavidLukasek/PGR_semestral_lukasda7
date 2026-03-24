#pragma once

#include "object.h"

enum LightType { POINT_LIGHT, SPOT_LIGHT, AREA_LIGHT };

class Light : public Object {
protected:
    float intensity;
    LightType lightType;
    glm::vec3 color;
    
public:
    explicit Light(float intensity, glm::vec3 color, LightType lightType)
        : Object()
        , intensity(intensity)
        , color(color) 
        , lightType(lightType) {
    }
    ~Light() override = default;
};
