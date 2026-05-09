//----------------------------------------------------------------------------------------
/**
 * \file       skydome.frag
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Skydome fragment shader.
 *
 *  Samples sky textures and computes final skydome color, including atmospheric blending effects.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Skydome fragment shader.
// Samples equirectangular sky texture and blends scene fog along view ray.

// ------------------------------- Uniforms -----------------------------------

uniform sampler2D diffuseTex; // skydome texture sampler
uniform vec3  cameraPosition; // camera position in world space
uniform vec3  fogCenter;      // center of primary fog sphere
uniform vec3  fogCenter2;     // center of secondary fog sphere
uniform vec3  fogColor;       // color of primary fog sphere
uniform vec3  fogColor2;      // color of secondary fog sphere
uniform float fogRadius;      // radius of primary fog sphere
uniform float fogRadius2;     // radius of secondary fog sphere
uniform float fogDensity;     // density multiplier for primary fog
uniform float fogDensity2;    // density multiplier for secondary fog

// ------------------------------- Attributes ---------------------------------

in vec2 theTexCoord; // texture coordinates on skydome
in vec3 thePosition; // direction vector used for fog ray

out vec4 fragmentColor; // final RGBA output

// ------------------------------- Functions ----------------------------------

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
    float densityScale =
        (fogRadialDensity(startR) +
         4.0 * fogRadialDensity(midR) +
         fogRadialDensity(endR)) / 6.0;

    return segmentLength * densityScale;
}

// Converts integrated fog amount to blend factor.
float fogFactorFromAmount(float fogAmount) {
    return 1.0 - exp(-fogAmount);
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // sample sky color from skydome texture
    vec3 skyColor = texture(diffuseTex, theTexCoord).rgb;
    vec3 rayDirection = normalize(thePosition);

    // integrate fog through infinite-looking ray toward the skydome
    float traveledInFog1 = fogSegmentLength(cameraPosition, rayDirection, -1.0, fogCenter, fogRadius);
    float traveledInFog2 = fogSegmentLength(cameraPosition, rayDirection, -1.0, fogCenter2, fogRadius2);
    float fogAmount1 = traveledInFog1 * fogDensity;
    float fogAmount2 = traveledInFog2 * fogDensity2;
    float totalFogAmount = fogAmount1 + fogAmount2;
    // mix sky with fog contribution from both fog volumes
    vec3 mixedFogColor = (totalFogAmount > 0.0)
        ? (fogColor * fogAmount1 + fogColor2 * fogAmount2) / totalFogAmount
        : fogColor;
    float fogFactor = fogFactorFromAmount(totalFogAmount);
    vec3 finalColor = mix(skyColor, mixedFogColor, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}
