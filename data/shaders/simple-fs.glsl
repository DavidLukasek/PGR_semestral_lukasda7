#version 140

out vec4 fragmentColor;

void main() {
	const float WIN_HEIGHT = 500.0;

	fragmentColor = vec4(gl_FragCoord.xy / WIN_HEIGHT, 0.0, 1.0);
}
