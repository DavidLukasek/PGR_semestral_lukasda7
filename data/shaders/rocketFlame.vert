#version 140

// -------------------------------- Macros ------------------------------------

#define FRAME_TIME 0.01
#define UV_OFFSET 0.25
#define NUM_FRAMES 4

// ------------------------------- Uniforms -----------------------------------

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform float elapsedTime;
uniform vec3 cameraPosition;

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
    // camera-facing billboard constrained to the local Z axis
    vec3 objectPosition = vec3(modelMatrix * vec4(0.0, 0.0, 0.0, 1.0));
    mat3 modelRotationScale = mat3(modelMatrix);

    vec3 localXWorld = modelRotationScale[0];
    vec3 localYWorld = modelRotationScale[1];
    vec3 localZWorld = modelRotationScale[2];

    float scaleX = max(length(localXWorld), 0.0001);
    float scaleY = max(length(localYWorld), 0.0001);
    float scaleZ = max(length(localZWorld), 0.0001);

    vec3 axisZ = localZWorld / scaleZ;

    vec3 toCamera = cameraPosition - objectPosition;
    vec3 projectedToCamera = toCamera - axisZ * dot(toCamera, axisZ);

    vec3 billboardY = normalize(localYWorld);
    if (dot(projectedToCamera, projectedToCamera) > 0.000001) {
        billboardY = normalize(projectedToCamera);
    }

    vec3 billboardX = normalize(cross(billboardY, axisZ));
    billboardY = normalize(cross(axisZ, billboardX));

    vec3 worldPos = objectPosition
                  + billboardX * (position.x * scaleX)
                  + billboardY * (position.y * scaleY)
                  + axisZ      * (position.z * scaleZ);

    gl_Position = projectionMatrix * viewMatrix * vec4(worldPos, 1.0);
    worldPosition = worldPos;
    worldNormal = normalize(billboardX * normal.x + billboardY * normal.y + axisZ * normal.z);

    // sprite animation using UV shift
    int frame = int(elapsedTime / FRAME_TIME) % NUM_FRAMES;
    theTexCoord = vec2(texCoord.x + float(frame) * UV_OFFSET, texCoord.y);
}
