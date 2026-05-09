//----------------------------------------------------------------------------------------
/**
 * \file       mandelbrot.vert
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Mandelbrot vertex shader.
 *
 *  Prepares fullscreen geometry and varyings required for Mandelbrot fractal rendering.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Mandelbrot vertex shader.
// Pass-through transform for board mesh and UV propagation.

// ------------------------------- Uniforms -----------------------------------

uniform mat4 pvmMatrix; // projection * view * model matrix

// ------------------------------- Attributes ---------------------------------

in vec3 position; // vertex position in object space
in vec2 texCoord; // vertex texture coordinates

out vec2 UV; // propagated texture coordinates

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // project vertex to clip space and pass UVs to fragment shader
    gl_Position = pvmMatrix * vec4(position, 1.0);
    UV = texCoord;
}
