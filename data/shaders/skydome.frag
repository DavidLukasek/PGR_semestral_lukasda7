#version 140

// ------------------------------- Uniforms -----------------------------------

uniform sampler2D diffuseTex;
uniform vec3  cameraPosition;
uniform vec3  fogCenter;
uniform vec3  fogCenter2;
uniform vec3  fogColor;
uniform vec3  fogColor2;
uniform float fogRadius;
uniform float fogRadius2;
uniform float fogDensity;
uniform float fogDensity2;

// ------------------------------- Attributes ---------------------------------

in vec2 theTexCoord;
in vec3 thePosition;

out vec4 fragmentColor;

// ------------------------------- Functions ----------------------------------

float fogRadialDensity(float normalizedRadius) {
    // keep a soft outer boundary for smoother transition to empty space
    return 1.0 - smoothstep(0.72, 1.0, normalizedRadius);
}

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

float fogFactorFromAmount(float fogAmount) {
    return 1.0 - exp(-fogAmount);
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    vec3 skyColor = texture(diffuseTex, theTexCoord).rgb;
    vec3 rayDirection = normalize(thePosition);

    float traveledInFog1 = fogSegmentLength(cameraPosition, rayDirection, -1.0, fogCenter, fogRadius);
    float traveledInFog2 = fogSegmentLength(cameraPosition, rayDirection, -1.0, fogCenter2, fogRadius2);
    float fogAmount1 = traveledInFog1 * fogDensity;
    float fogAmount2 = traveledInFog2 * fogDensity2;
    float totalFogAmount = fogAmount1 + fogAmount2;
    vec3 mixedFogColor = (totalFogAmount > 0.0)
        ? (fogColor * fogAmount1 + fogColor2 * fogAmount2) / totalFogAmount
        : fogColor;
    float fogFactor = fogFactorFromAmount(totalFogAmount);
    vec3 finalColor = mix(skyColor, mixedFogColor, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}
