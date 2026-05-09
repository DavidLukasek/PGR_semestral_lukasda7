//----------------------------------------------------------------------------------------
/**
 * \file       phong.vert
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Phong vertex shader.
 *
 *  Transforms geometry and passes lighting-relevant attributes to the Phong fragment stage.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Phong vertex shader.
// Transforms mesh vertices to clip space and passes world-space data to fragment stage.

// ------------------------------- Uniforms -----------------------------------

uniform mat4 modelMatrix;   // model -> world transform
uniform mat4 normalMatrix;  // inverse-transpose model matrix for normal transform
uniform mat4 pvmMatrix;     // projection * view * model matrix

uniform float elapsedTime;  // elapsed application time (seconds)
uniform int   isUVAnimated; // enables scrolling UV animation

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
    // project vertex to clip space
    gl_Position = pvmMatrix * vec4(position, 1.0);

    // pass world-space position and transformed normal to fragment shader
    worldPosition = vec3(modelMatrix * vec4(position, 1.0));
    worldNormal = normalize(vec3(normalMatrix * vec4(normal, 0.0)));

    // optionally animate UVs by a uniform scrolling offset
    theTexCoord = texCoord;
    if (bool(isUVAnimated)) theTexCoord += 0.1*elapsedTime;
}
