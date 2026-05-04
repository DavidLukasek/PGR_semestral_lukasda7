#version 140

// ------------------------------- Uniforms -----------------------------------

uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform mat4 pvmMatrix;

uniform float elapsedTime;
uniform float displacementSize;
uniform bool isDisplaceAnimated;

// ------------------------------- Attributes ---------------------------------

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 theTexCoord;

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    vec3 dir = normalize(position);
    float t = elapsedTime;

    // lightweight pseudo-random displacement assembled from several
    // directional waves with non-matching frequencies
    float n1 = sin(dot(dir, normalize(vec3( 1.0,  0.3,  0.7))) * 27.0 + t * 1.75);
    float n2 = sin(dot(dir, normalize(vec3(-0.6,  1.0,  0.2))) * 21.0 - t * 1.41);
    float n3 = sin(dot(dir, normalize(vec3( 0.2, -0.7,  1.0))) * 33.0 + t * 1.12);
    float n4 = sin(dot(dir, normalize(vec3( -0.4, 0.7, -0.7))) *  7.0 + t * 1.75);
    float n5 = sin(dot(dir, normalize(vec3( 0.3,  0.2, -0.4))) *  5.0 - t * 1.41);
    float n6 = sin(dot(dir, normalize(vec3( -0.1, -1.7, 0.3))) *  3.0 + t * 1.12);

    float wave = 0.41 * n1 + 0.27 * n2 + 0.18 * n3 +
                 0.27 * n4 + 0.31 * n5 + 0.21 * n6;

    // subtle ridge term (mountain-like streaks)
    /*float ridge = 1.0 - abs(sin(dot(dir, vec3(5.7, 8.3, 6.1)) * 12.0 - t * 0.24));
    wave += 0.35 * ridge - 0.18;*/

    // tiny domain warp for less periodic look
    /*vec3 warpedDir = normalize(dir + 0.38 * vec3(n2, n3, n1));
    float detail = sin(dot(warpedDir, vec3(13.1, 9.2, 15.7)) * 22.0 + t * 0.70);
    wave += 0.20 * detail;*/

    vec3 displacedPosition = position;
    if (isDisplaceAnimated) {
        displacedPosition = position + dir * (wave * displacementSize);
    }

    gl_Position = pvmMatrix * vec4(displacedPosition, 1.0);
    worldPosition = vec3(modelMatrix * vec4(displacedPosition, 1.0));

    vec3 displacedNormal = normalize(displacedPosition);
    worldNormal = normalize(vec3(normalMatrix * vec4(displacedNormal, 0.0)));

    theTexCoord = texCoord;
}
