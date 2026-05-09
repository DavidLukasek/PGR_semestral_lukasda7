//----------------------------------------------------------------------------------------
/**
 * \file       displacement.vert
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Displacement vertex shader.
 *
 *  Transforms mesh vertices and applies displacement-related logic before rasterization.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Displacement vertex shader.
// Animates mesh surface by moving vertices along radial direction.

// ------------------------------- Uniforms -----------------------------------

uniform mat4 modelMatrix;   // model -> world transform
uniform mat4 normalMatrix;  // inverse-transpose model matrix for normal transform
uniform mat4 pvmMatrix;     // projection * view * model matrix

uniform float elapsedTime;       // elapsed application time
uniform float displacementSize;  // displacement amplitude
uniform bool isDisplaceAnimated; // enables procedural displacement

// ------------------------------- Attributes ---------------------------------

in vec3 position; // vertex position in object space
in vec3 normal;   // vertex normal in object space
in vec2 texCoord; // vertex texture coordinates

out vec3 worldPosition; // fragment position in world space
out vec3 worldNormal;   // fragment normal in world space
out vec2 theTexCoord;   // propagated texture coordinates

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // normalized radial direction used for sphere-like deformation
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

    // project displaced vertex and output derived world-space data
    gl_Position = pvmMatrix * vec4(displacedPosition, 1.0);
    worldPosition = vec3(modelMatrix * vec4(displacedPosition, 1.0));

    vec3 displacedNormal = normalize(displacedPosition);
    worldNormal = normalize(vec3(normalMatrix * vec4(displacedNormal, 0.0)));

    // preserve base UV coordinates for texturing in fragment stage
    theTexCoord = texCoord;
}
