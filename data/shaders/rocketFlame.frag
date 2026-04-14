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

    return max(end - start, 0.0);
}

float fogFactorFromLength(float traveledInFog) {
    float core = clamp(traveledInFog * fogDensity, 0.0, 1.0);
    float edge = smoothstep(0.0, 1.0, traveledInFog);
    return core * edge;
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
