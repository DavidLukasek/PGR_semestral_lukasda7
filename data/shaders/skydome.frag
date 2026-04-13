#version 140

// ------------------------------- Uniforms -----------------------------------

uniform sampler2D diffuseTex;
uniform vec3  cameraPosition;
uniform vec3  fogCenter;
uniform vec3  fogColor;
uniform float fogRadius;
uniform float fogDensity;

// ------------------------------- Attributes ---------------------------------

in vec2 theTexCoord;
in vec3 thePosition;

out vec4 fragmentColor;

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
    vec3 skyColor = texture(diffuseTex, theTexCoord).rgb;
    vec3 rayDirection = normalize(thePosition);

    float traveledInFog = fogSegmentLength(cameraPosition, rayDirection, -1.0);
    float fogFactor = fogFactorFromLength(traveledInFog);
    vec3 finalColor = mix(skyColor, fogColor, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}
