//----------------------------------------------------------------------------------------
/**
 * \file       skydome.vert
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Skydome vertex shader.
 *
 *  Transforms skydome geometry and forwards coordinates for sky texture sampling and fog blending.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Skydome vertex shader.
// Passes dome position and UV to fragment shader for sky sampling and fog.

// ------------------------------- Uniforms -----------------------------------

uniform mat4 pvmMatrix; // projection * view * model matrix

// ------------------------------- Attributes ---------------------------------

in vec3 position; // vertex position in object space
in vec2 texCoord; // sky texture coordinates

out vec2 theTexCoord; // propagated texture coordinates
out vec3 thePosition; // propagated object-space direction for ray setup

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // project dome vertex to clip space and pass data to fragment shader
    gl_Position = pvmMatrix * vec4(position, 1.0);

    thePosition = position;
    
    theTexCoord = texCoord;
}
