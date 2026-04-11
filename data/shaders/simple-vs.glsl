#version 140

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;
in vec2 texCoord;

out vec2 UV;

void main() {
    mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;

    gl_Position = PVM * vec4(position, 1.0);
    UV = texCoord;
}
