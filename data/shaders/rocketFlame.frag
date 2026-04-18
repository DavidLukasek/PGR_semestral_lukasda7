#version 140

// ------------------------------- Uniforms -----------------------------------

uniform sampler2D diffuseTex;
uniform vec3  asteroidLocation;
uniform vec3  cameraPosition;

// fog uniforms
uniform vec3  fogCenter;
uniform vec3  fogColor;
uniform float fogRadius;
uniform float fogDensity;

// ------------------------------- Attributes ---------------------------------

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 theTexCoord;

out vec4 fragmentColor;

// ------------------------------- Functions ----------------------------------

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
    float densityScale =
        (fogRadialDensity(startR) +
         4.0 * fogRadialDensity(midR) +
         fogRadialDensity(endR)) / 6.0;

    return segmentLength * densityScale;
}

float fogFactorFromLength(float traveledInFog) {
    return 1.0 - exp(-traveledInFog * fogDensity);
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // world-coordinates position and normal of fragment
    vec3 position = worldPosition;
    vec3 normal = normalize(worldNormal);
    
    vec4 texel = texture(diffuseTex, theTexCoord);

    // black background works as a color key with a soft edge
    float key = max(texel.r, max(texel.g, texel.b));
    float colorKeyAlpha = smoothstep(0.0, 1.0, key);
    float alpha = texel.a * colorKeyAlpha;

    // not even drawing transparent enough fragments
    if (alpha < 0.01)
        discard;

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
    float fogFactor = fogFactorFromLength(traveledInFog);
    texel.rgb = mix(texel.rgb, fogColor, fogFactor);

    fragmentColor = vec4(texel.rgb, alpha);
}
