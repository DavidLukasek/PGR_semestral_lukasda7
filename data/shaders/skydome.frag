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
    vec3 skyColor = texture(diffuseTex, theTexCoord).rgb;
    vec3 rayDirection = normalize(thePosition);

    float traveledInFog = fogSegmentLength(cameraPosition, rayDirection, -1.0);
    float fogFactor = fogFactorFromLength(traveledInFog);
    vec3 finalColor = mix(skyColor, fogColor, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}
