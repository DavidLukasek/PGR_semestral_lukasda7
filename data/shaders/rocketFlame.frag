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

    // black background works as a color key with a soft edge
    float key = max(texel.r, max(texel.g, texel.b));
    float colorKeyAlpha = smoothstep(0.0, 1.0, key);
    float alpha = texel.a * colorKeyAlpha;

    // not even drawing transparent enough fragments
    if (alpha < 0.01)
        discard;

    fragmentColor = vec4(texel.rgb, alpha);
}
