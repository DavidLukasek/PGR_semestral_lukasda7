#version 140

// -------------------------------- Macros ------------------------------------

#define FRAME_TIME 0.1
#define UV_OFFSET 0.25
#define NUM_FRAMES 4

// ------------------------------- Uniforms -----------------------------------

uniform mat4 PVM;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float elapsedTime;

// ------------------------------- Attributes ---------------------------------

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 theTexCoord;

// ############################################################################
//                                  Main
// ############################################################################

// TODO: SET INVERSES AND STUFF AS UNIFORMS !!!

void main() {
    // camera-facing billboard constrained to Y axis
    vec3 objectPosition = modelMatrix[3].xyz;
    vec3 cameraPosition = inverse(viewMatrix)[3].xyz;

    mat4 PV = PVM * inverse(modelMatrix);

    vec3 toCamera = cameraPosition - objectPosition;
    toCamera.y = 0.0;

    toCamera = normalize(toCamera);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, toCamera));

    // assumption: quad stays in local XY plane and has z = 0
    vec3 worldPos = objectPosition
                  + right * position.x
                  + up    * position.y;

    gl_Position = PV * vec4(worldPos, 1.0);

    // sprite animation using UV shift
    int frame = int(elapsedTime / FRAME_TIME) % NUM_FRAMES;
    theTexCoord = vec2(texCoord.x + float(frame) * UV_OFFSET, texCoord.y);
}
