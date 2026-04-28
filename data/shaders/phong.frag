#version 140

// -------------------------------- Macros ------------------------------------

// light types for better distinction
#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTION_LIGHT 2

#define MAX_SCENE_LIGHTS 8

// ------------------------------- Uniforms -----------------------------------

// misc uniforms
uniform float elapsedTime;          // elapsed application time
uniform vec3  ambientColor;         // ambient color
uniform vec3  cameraPosition;       // position of the camera
uniform int   hasDiffuseTexture;    // flag for material with diffuse texture
uniform int   hasNormalTexture;     // flag for material with normal texture
uniform int   hasSpecularTexture;   // flag for material with specular texture

// matrix uniforms
uniform mat4  projectionMatrix;     // projection matrix
uniform mat4  viewMatrix;           // view matrix
uniform mat4  modelMatrix;          // model matrix
uniform mat4  normalMatrix;         // inverse transposed model matrix

// texture sampler uniforms
uniform sampler2D diffuseSampler;   // diffuse texture sampler
uniform sampler2D normalSampler;    // normal texture sampler
uniform sampler2D specularSampler;  // specular texture sampler

// current material uniforms
uniform vec3  matDiffuse;           // diffuse parameter of the material
uniform vec3  matSpecular;          // specular parameter of the material
uniform vec3  matAmbient;           // ambient parameter of the material
uniform float matShininess;         // shininess parameter of the material

// fog uniforms
uniform vec3  fogCenter;            // fog center
uniform vec3  fogColor;             // fog color
uniform float fogRadius;            // fog radius
uniform float fogDensity;           // fog density

// all scene lights uniforms
uniform int   lightCount;
uniform int   lightTypes[MAX_SCENE_LIGHTS];
uniform vec3  lightAmbients[MAX_SCENE_LIGHTS];
uniform vec3  lightDiffuses[MAX_SCENE_LIGHTS];
uniform vec3  lightSpeculars[MAX_SCENE_LIGHTS];
uniform vec3  lightPositions[MAX_SCENE_LIGHTS];
uniform vec3  lightSpotDirections[MAX_SCENE_LIGHTS];
uniform float lightSpotCutOffs[MAX_SCENE_LIGHTS];
uniform float lightSpotExponents[MAX_SCENE_LIGHTS];
uniform float lightIntensities[MAX_SCENE_LIGHTS];


// ------------------------------- Attributes ---------------------------------

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 theTexCoord;

out vec4 fragmentColor;

// ------------------------------- Functions ----------------------------------

vec3 getColorFromLight(vec3 albedo, vec3 specularStrength, vec3 position, vec3 normal, int index) {
    vec3 lightVec = lightPositions[index] - position;
    float lightDist = length(lightVec);
    float lightFallof = 1.0 / (lightDist * lightDist);

    vec3 L = normalize(lightVec);

    // oprava directional light
    if (lightTypes[index] == DIRECTION_LIGHT) {
        L = normalize(-lightSpotDirections[index]);
        lightFallof = 1.0;
    }

    vec3 R = reflect(-L, normal);
    vec3 V = normalize(cameraPosition - position);

    float NdotL = max(dot(normal, L), 0.0);
    float spotCos = dot(normalize(lightSpotDirections[index]), -L);
    float spotEffect = pow(max(spotCos, 0.0), lightSpotExponents[index]);
    float cosB = max(dot(R, V), 0.0);

    vec3 ambient = lightAmbients[index] * matAmbient * albedo;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (NdotL <= 0.0) return ambient;

    // calculating light based on the light type
    switch (lightTypes[index]) {
        // point light calculation
        case POINT_LIGHT:
            diffuse = NdotL * lightDiffuses[index] *
                      matDiffuse * albedo * lightFallof;
            specular = pow(cosB, matShininess) * lightSpeculars[index] *
                       matSpecular * specularStrength * lightFallof;
            break;

        // spot light calculation
        case SPOT_LIGHT:
            if (spotCos < lightSpotCutOffs[index])
                return ambient;

            diffuse = NdotL * spotEffect * lightDiffuses[index] *
                      matDiffuse * albedo * lightFallof;
            specular = spotEffect * pow(cosB, matShininess) *
                       lightSpeculars[index] * matSpecular * specularStrength * lightFallof;
            break;

        // directional light calculation
        case DIRECTION_LIGHT:
            diffuse = NdotL * lightDiffuses[index] *
                      matDiffuse * albedo;
            specular = pow(cosB, matShininess) *
                       lightSpeculars[index] * matSpecular * specularStrength;
            break;
    }

    return ambient + lightIntensities[index] * (diffuse + specular);
}

float fogRadialDensity(float normalizedRadius) {
    // keep a soft outer boundary for smoother transition to empty space
    return 1.0 - smoothstep(0.72, 1.0, normalizedRadius);
}

float fogSegmentLength(vec3 rayOrigin, vec3 rayDirection, float maxDistance) {
    vec3 oc = rayOrigin - fogCenter;
    float b = dot(oc, rayDirection);
    float c = dot(oc, oc) - fogRadius * fogRadius;
    float h = b * b - c;

    if (h <= 0.0)
        return 0.0;

    float sqrtH = sqrt(h);
    float tNear = -b - sqrtH;
    float tFar = -b + sqrtH;

    float start = max(tNear, 0.0);
    float end = tFar;

    if (maxDistance > 0.0)
        end = min(end, maxDistance);

    float segmentLength = max(end - start, 0.0);
    if (segmentLength <= 0.0)
        return 0.0;

    float invFogRadius = 1.0 / max(fogRadius, 0.0001);
    vec3 startPos = rayOrigin + rayDirection * start;
    vec3 endPos = rayOrigin + rayDirection * end;
    vec3 midPos = 0.5 * (startPos + endPos);

    float startR = length(startPos - fogCenter) * invFogRadius;
    float midR = length(midPos - fogCenter) * invFogRadius;
    float endR = length(endPos - fogCenter) * invFogRadius;

    // Simpson approximation of density integral along ray segment
    float densityScale = (fogRadialDensity(startR) +
                          4.0 * fogRadialDensity(midR) +
                          fogRadialDensity(endR))
                          / 6.0;

    return segmentLength * densityScale;
}

vec3 getNormalFromMap(vec3 position, vec3 baseNormal, vec2 texCoord) {
    vec3 tangentNormal = texture(normalSampler, texCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(position);
    vec3 Q2  = dFdy(position);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    vec3 N = normalize(baseNormal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = normalize(-Q1 * st2.s + Q2 * st1.s);

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // world-coordinates position and normal of fragment
    vec3 position = worldPosition;
    vec3 baseNormal = normalize(worldNormal);

    // diffuse texture color
    vec3 albedo = vec3(1.0);
    float alpha = 1.0;
    if (hasDiffuseTexture != 0) {
        vec4 texel = texture(diffuseSampler, theTexCoord);
        albedo = texel.rgb;
        alpha = texel.w;
    }

    // normal texture color
    vec3 normal = baseNormal;
    if (hasNormalTexture != 0) {
        normal = getNormalFromMap(position, baseNormal, theTexCoord);
    }

    // specular texture color
    vec3 specularStrength = vec3(1.0);
    if (hasSpecularTexture != 0) {
        vec4 texel = texture(specularSampler, theTexCoord);
        specularStrength = texel.rgb;
    }

    // ambient * diffuse color
    vec3 color = ambientColor * matAmbient * albedo;

    // adding color from all lights in the scene
    for (int i = 0; i < lightCount; i++) {
        color += getColorFromLight(albedo, specularStrength, position, normal, i);
    }

    // setup for fog
    vec3 viewVector = position - cameraPosition;
    float maxDistance = length(viewVector);
    float traveledInFog = 0.0;

    // getting fog view thickness
    if (maxDistance > 0.0) {
        vec3 rayDirection = viewVector / maxDistance;
        traveledInFog = fogSegmentLength(cameraPosition, rayDirection, maxDistance);
    }

    // adding fog to the color
    float fogFactor = 1.0 - exp(-traveledInFog * fogDensity);
    color = mix(color, fogColor, fogFactor);

    fragmentColor = vec4(color, alpha);
}
