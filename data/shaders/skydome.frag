#version 140

// ------------------------------- Uniforms -----------------------------------

uniform sampler2D diffuseTex;

// ------------------------------- Attributes ---------------------------------

in vec2 theTexCoord;

out vec4 fragmentColor;

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    vec4 texel = texture(diffuseTex, theTexCoord);

    fragmentColor = vec4(texel.rgb, 1.0);
}
