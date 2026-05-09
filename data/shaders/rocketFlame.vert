//----------------------------------------------------------------------------------------
/**
 * \file       rocketFlame.vert
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Rocket flame vertex shader.
 *
 *  Processes flame billboard or mesh vertices and forwards animation inputs to the fragment stage.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Rocket flame vertex shader.
// Builds axis-constrained billboard and animates sprite-sheet UV frames.

// -------------------------------- Macros ------------------------------------

#define FRAME_TIME 0.01
#define UV_OFFSET 0.25
#define NUM_FRAMES 4

// ------------------------------- Uniforms -----------------------------------

uniform mat4 modelMatrix;   // model -> world transform
uniform mat4 pvmMatrix;     // projection * view * model matrix
uniform float elapsedTime;  // elapsed application time
uniform vec3 cameraPosition;// camera position in world space

// ------------------------------- Attributes ---------------------------------

in vec3 position; // vertex position in object space
in vec3 normal;   // vertex normal in object space
in vec2 texCoord; // vertex texture coordinates

out vec3 worldPosition; // fragment position in world space
out vec3 worldNormal;   // billboard-oriented normal in world space
out vec2 theTexCoord;   // animated sprite UV coordinates

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

    // reconstruct world-space vertex on billboard plane with preserved local scale
    vec3 worldPos = objectPosition
                  + billboardX * (position.x * scaleX)
                  + billboardY * (position.y * scaleY)
                  + axisZ      * (position.z * scaleZ);

    // project transformed vertex and output interpolated per-fragment data
    gl_Position = pvmMatrix * vec4(worldPos, 1.0);
    worldPosition = worldPos;
    worldNormal = normalize(billboardX * normal.x + billboardY * normal.y + axisZ * normal.z);

    // sprite animation using UV shift
    int frame = int(elapsedTime / FRAME_TIME) % NUM_FRAMES;
    theTexCoord = vec2(texCoord.x + float(frame) * UV_OFFSET, texCoord.y);
}
