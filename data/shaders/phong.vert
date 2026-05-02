#version 140

// ------------------------------- Uniforms -----------------------------------

uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform mat4 pvmMatrix;

uniform float elapsedTime;
uniform int   isUVAnimated;

// ------------------------------- Attributes ---------------------------------

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 theTexCoord;

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    gl_Position = pvmMatrix * vec4(position, 1.0);

    worldPosition = vec3(modelMatrix * vec4(position, 1.0));
    worldNormal = normalize(vec3(normalMatrix * vec4(normal, 0.0)));

    theTexCoord = texCoord;
    if (bool(isUVAnimated)) theTexCoord += 0.1*elapsedTime;
}
