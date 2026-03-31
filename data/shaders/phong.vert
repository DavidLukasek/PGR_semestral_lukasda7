#version 140

uniform mat4 PVM;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform float elapsedTime;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 thePosition;
out vec3 theNormal;
out vec2 theTexCoord;

void main() {
    gl_Position = PVM * vec4(position, 1.0);

    thePosition = vec3(modelMatrix * vec4(position, 1.0));
    theNormal = normalize(vec3(normalMatrix * vec4(normal, 0.0)));
    theTexCoord = texCoord;
}
