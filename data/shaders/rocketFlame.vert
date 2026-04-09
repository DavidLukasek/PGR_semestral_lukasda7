#version 140

// -------------------------------- Macros ------------------------------------

#define FRAME_TIME 0.1
#define UV_OFFSET 0.25
#define NUM_FRAMES 4

// ------------------------------- Uniforms -----------------------------------

uniform mat4 PVM;
uniform float elapsedTime;

// ------------------------------- Attributes ---------------------------------

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 theTexCoord;

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    gl_Position = PVM * vec4(position, 1.0);

    int frame = int(elapsedTime / FRAME_TIME) % NUM_FRAMES;

    float offsetTexCoordX = texCoord.x + frame*UV_OFFSET;

    theTexCoord = vec2(offsetTexCoordX, texCoord.y);
}
