#version 140

in vec2 position;

uniform mat4 PVM;

void main() {
	gl_Position = PVM * vec4(position, 0.0, 1.0);
}