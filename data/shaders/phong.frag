#version 140

const int MAX_SCENE_LIGHTS = 16;

uniform float elapsedTime;
uniform int lightCount;
uniform vec3 lightPositions[MAX_SCENE_LIGHTS];
uniform vec3 lightColors[MAX_SCENE_LIGHTS];
uniform float lightIntensities[MAX_SCENE_LIGHTS];
uniform int lightTypes[MAX_SCENE_LIGHTS];

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 fragmentColor;

void main() {
    vec3 color = vNormal;

    fragmentColor = vec4(color, 1.0);
}
