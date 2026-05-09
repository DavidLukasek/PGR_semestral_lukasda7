//----------------------------------------------------------------------------------------
/**
 * \file       rocketFlame.frag
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Rocket flame fragment shader.
 *
 *  Generates animated rocket flame color and transparency effects for a dynamic exhaust look.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Rocket flame fragment shader.
// Samples animated flame sprite, applies color-key alpha and volumetric fog.

// ------------------------------- Uniforms -----------------------------------

uniform sampler2D diffuseTex;     // flame sprite-sheet texture
uniform vec3  asteroidLocation;   // reserved uniform kept for program compatibility
uniform vec3  cameraPosition;     // camera position in world space

// fog uniforms
uniform vec3  fogCenter;   // center of primary fog sphere
uniform vec3  fogCenter2;  // center of secondary fog sphere
uniform vec3  fogColor;    // color of primary fog sphere
uniform vec3  fogColor2;   // color of secondary fog sphere
uniform float fogRadius;   // radius of primary fog sphere
uniform float fogRadius2;  // radius of secondary fog sphere
uniform float fogDensity;  // density multiplier for primary fog
uniform float fogDensity2; // density multiplier for secondary fog

// ------------------------------- Attributes ---------------------------------

in vec3 worldPosition; // fragment position in world space
in vec3 worldNormal;   // fragment normal in world space (unused by current lighting)
in vec2 theTexCoord;   // texture coordinates for sprite-sheet sampling

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
    // world-coordinates position and normal of fragment
    vec3 position = worldPosition;
    vec3 normal = normalize(worldNormal);
    
    // sample flame sprite frame
    vec4 texel = texture(diffuseTex, theTexCoord);

    // black background works as a color key with a soft edge
    float key = max(texel.r, max(texel.g, texel.b));
    float colorKeyAlpha = smoothstep(0.0, 1.0, key);
    float alpha = texel.a * colorKeyAlpha;

    // skip almost fully transparent fragments to reduce overdraw
    if (alpha < 0.01)
        discard;

    // setup for fog
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

    // adding fog to the color
    float fogAmount1 = traveledInFog1 * fogDensity;
    float fogAmount2 = traveledInFog2 * fogDensity2;
    float totalFogAmount = fogAmount1 + fogAmount2;
    vec3 mixedFogColor = (totalFogAmount > 0.0)
        ? (fogColor * fogAmount1 + fogColor2 * fogAmount2) / totalFogAmount
        : fogColor;
    float fogFactor = fogFactorFromAmount(totalFogAmount);
    texel.rgb = mix(texel.rgb, mixedFogColor, fogFactor);

    fragmentColor = vec4(texel.rgb, alpha);
}
