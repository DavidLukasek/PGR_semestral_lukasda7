#version 140

uniform mat4 PVM;

in vec3 position;
in vec2 texCoord;

out vec2 UV;


void main() {
    gl_Position = PVM * vec4(position, 1.0);
    UV = texCoord;
}
