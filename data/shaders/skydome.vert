#version 140

// ------------------------------- Uniforms -----------------------------------

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

// ------------------------------- Attributes ---------------------------------

in vec3 position;
in vec2 texCoord;

out vec2 theTexCoord;
out vec3 thePosition;

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    mat4 PVM = projectionMatrix * mat4(mat3(viewMatrix));

    gl_Position = PVM * vec4(position, 1.0);

    thePosition = position;
    
    theTexCoord = texCoord;
}
