//----------------------------------------------------------------------------------------
/**
 * \file       phong.frag
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Phong fragment shader.
 *
 *  Calculates per-fragment Phong lighting with textures, reflections and atmospheric effects.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Phong fragment shader.
// Computes textured lighting, reflections and volumetric fog blending.

// -------------------------------- Macros ------------------------------------

// light types for better distinction
#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTION_LIGHT 2

#define PI 3.14159265359

#define MAX_SCENE_LIGHTS 10

// ------------------------------- Uniforms -----------------------------------

// misc uniforms
uniform float elapsedTime;          // elapsed application time
uniform vec3  ambientColor;         // global ambient lighting color
uniform vec3  cameraPosition;       // camera position in world space
uniform int   hasDiffuseTexture;    // flag for diffuse texture sampling
uniform int   hasNormalTexture;     // flag for normal map sampling
uniform int   hasSpecularTexture;   // flag for specular map sampling

// texture sampler uniforms
uniform sampler2D diffuseSampler;   // diffuse texture sampler
uniform sampler2D normalSampler;    // normal texture sampler
uniform sampler2D specularSampler;  // specular texture sampler
uniform sampler2D environmentSampler; // environment texture sampler

// current material uniforms
uniform vec3  matDiffuse;           // material diffuse color multiplier
uniform vec3  matSpecular;          // material specular color multiplier
uniform vec3  matAmbient;           // material ambient color multiplier
uniform float matShininess;         // specular highlight exponent

// fog uniforms
uniform vec3  fogCenter;            // fog center
uniform vec3  fogCenter2;           // second fog center
uniform vec3  fogColor;             // fog color
uniform vec3  fogColor2;            // second fog color
uniform float fogRadius;            // fog radius
uniform float fogRadius2;           // second fog radius
uniform float fogDensity;           // fog density
uniform float fogDensity2;          // second fog density

// all scene lights uniforms
uniform int   lightCount;                        // number of active lights
uniform int   lightTypes[MAX_SCENE_LIGHTS];      // POINT/SPOT/DIRECTIONAL type
uniform vec3  lightAmbients[MAX_SCENE_LIGHTS];   // ambient component per light
uniform vec3  lightDiffuses[MAX_SCENE_LIGHTS];   // diffuse component per light
uniform vec3  lightSpeculars[MAX_SCENE_LIGHTS];  // specular component per light
uniform vec3  lightPositions[MAX_SCENE_LIGHTS];  // world-space position per light
uniform vec3  lightSpotDirections[MAX_SCENE_LIGHTS]; // spotlight direction per light
uniform float lightSpotCutOffs[MAX_SCENE_LIGHTS];    // spotlight cosine cutoff
uniform float lightSpotExponents[MAX_SCENE_LIGHTS];  // spotlight focus exponent
uniform float lightIntensities[MAX_SCENE_LIGHTS];    // scalar intensity per light


// ------------------------------- Attributes ---------------------------------

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 theTexCoord;

out vec4 fragmentColor;

// ------------------------------- Functions ----------------------------------

// Returns ambient + diffuse + specular contribution of one light source.
vec3 getColorFromLight(vec3 albedo, vec3 specularStrength, vec3 position, vec3 normal, int index) {
    vec3 lightVec = lightPositions[index] - position;
    float lightDist = length(lightVec);
    float lightFallof = 1.0 / (lightDist * lightDist);

    vec3 L = normalize(lightVec);

    // directional light uses uniform direction and no distance attenuation
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

// Returns radial density profile for a normalized fog radius.
float fogRadialDensity(float normalizedRadius) {
    // keep a soft outer boundary for smoother transition to empty space
    return 1.0 - smoothstep(0.72, 1.0, normalizedRadius);
}

// Approximates integrated fog density along ray segment inside one fog sphere.
float fogSegmentLength(vec3 rayOrigin, vec3 rayDirection, float maxDistance, vec3 center, float radius) {
    vec3 oc = rayOrigin - center;
    float b = dot(oc, rayDirection);
    float c = dot(oc, oc) - radius * radius;
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

    float invFogRadius = 1.0 / max(radius, 0.0001);
    vec3 startPos = rayOrigin + rayDirection * start;
    vec3 endPos = rayOrigin + rayDirection * end;
    vec3 midPos = 0.5 * (startPos + endPos);

    float startR = length(startPos - center) * invFogRadius;
    float midR = length(midPos - center) * invFogRadius;
    float endR = length(endPos - center) * invFogRadius;

    // Simpson approximation of density integral along ray segment
    float densityScale = (fogRadialDensity(startR) +
                          4.0 * fogRadialDensity(midR) +
                          fogRadialDensity(endR))
                          / 6.0;

    return segmentLength * densityScale;
}

// Reconstructs world-space normal from normal map using derivative-based TBN.
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

// Converts direction vector to equirectangular environment map UV.
vec2 getEquirectangularUV(vec3 direction) {
    vec3 dir = normalize(direction);

    // keep orientation consistent with the skydome UV layout
    float u = 0.5 - atan(dir.z, dir.x) / (2.0 * PI);
    float v = 0.5 + asin(clamp(dir.y, -1.0, 1.0)) / PI;

    return vec2(fract(u), clamp(v, 0.0, 1.0));
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // base fragment data
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

    // accumulate all scene light contributions
    for (int i = 0; i < lightCount; i++) {
        color += getColorFromLight(albedo, specularStrength, position, normal, i);
    }

    // add environment reflection weighted by Fresnel-like term
    vec3 V = normalize(cameraPosition - position);
    vec3 reflectionDirection = reflect(-V, normal);
    vec2 envTexCoord = getEquirectangularUV(reflectionDirection);
    vec3 envColor = texture(environmentSampler, envTexCoord).rgb;
    float fresnel = pow(1.0 - max(dot(normal, V), 0.0), 5.0);
    float reflectionStrength = (0.08 + 0.35 * fresnel) * dot(specularStrength, vec3(0.3333333));
    color += envColor * reflectionStrength;

    // compute fog traveled lengths from camera to fragment
    vec3 viewVector = position - cameraPosition;
    float maxDistance = length(viewVector);
    float traveledInFog1 = 0.0;
    float traveledInFog2 = 0.0;

    // getting fog view thickness
    if (maxDistance > 0.0) {
        vec3 rayDirection = viewVector / maxDistance;
        traveledInFog1 = fogSegmentLength(cameraPosition, rayDirection, maxDistance, fogCenter, fogRadius);
        traveledInFog2 = fogSegmentLength(cameraPosition, rayDirection, maxDistance, fogCenter2, fogRadius2);
    }

    // blend final color with fog
    float fogAmount1 = traveledInFog1 * fogDensity;
    float fogAmount2 = traveledInFog2 * fogDensity2;
    float totalFogAmount = fogAmount1 + fogAmount2;
    vec3 mixedFogColor = (totalFogAmount > 0.0)
        ? (fogColor * fogAmount1 + fogColor2 * fogAmount2) / totalFogAmount
        : fogColor;
    float fogFactor = 1.0 - exp(-totalFogAmount);
    color = mix(color, mixedFogColor, fogFactor);

    fragmentColor = vec4(color, alpha);
}
