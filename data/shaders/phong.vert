#version 140

uniform mat4 PVM;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform float elapsedTime;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 theTexCoord;

void main() {
    gl_Position = PVM * vec4(position, 1.0);

    worldPosition = vec3(modelMatrix * vec4(position, 1.0));
    worldNormal = normalize(vec3(normalMatrix * vec4(normal, 0.0)));
    theTexCoord = texCoord;
}
