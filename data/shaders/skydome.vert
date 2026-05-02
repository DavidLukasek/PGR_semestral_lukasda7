#version 140

// ------------------------------- Uniforms -----------------------------------

uniform mat4 pvmMatrix;

// ------------------------------- Attributes ---------------------------------

in vec3 position;
in vec2 texCoord;

out vec2 theTexCoord;
out vec3 thePosition;

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    gl_Position = pvmMatrix * vec4(position, 1.0);

    thePosition = position;
    
    theTexCoord = texCoord;
}
