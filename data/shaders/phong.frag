#version 140

// light types for better distinction
#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTION_LIGHT 2

#define MAX_SCENE_LIGHTS 16

// ------------------------------- Structs ------------------------------------

struct Material {       // structure that describes currently used material
    vec3  ambient;      // ambient component
    vec3  diffuse;      // diffuse component
    vec3  specular;     // specular component
    float shininess;    // sharpness of specular reflection
    bool  useTexture;   // defines whether the texture is used or not
};

struct Light {              // structure describing light parameters
    int   type;
    vec3  ambient;          // intensity & color of the ambient component
    vec3  diffuse;          // intensity & color of the diffuse component
    vec3  specular;         // intensity & color of the specular component
    vec3  position;         // light position
    vec3  spotDirection;    // spotlight direction
    float spotCosCutOff;    // cosine of the spotlight's half angle
    float spotExponent;     // distribution of the light energy within the spotlight's cone (center -> cone's edge)
};

// ------------------------------- Uniforms -----------------------------------

uniform float elapsedTime;      // elapsed application time
uniform vec3 ambientColor;      // ambient color
uniform vec3 cameraPosition;    // position of the camera

uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform vec3 matAmbient;
uniform float matShininess;

uniform mat4 PVMmatrix;         // Projection * View * Model  --> model to clip coordinates
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;      // inverse transposed model matrix

// light-related uniforms
uniform int   lightTypes[MAX_SCENE_LIGHTS];
uniform vec3  lightAmbients[MAX_SCENE_LIGHTS];
uniform vec3  lightDiffuses[MAX_SCENE_LIGHTS];
uniform vec3  lightSpeculars[MAX_SCENE_LIGHTS];
uniform vec3  lightPositions[MAX_SCENE_LIGHTS];
uniform vec3  lightSpotDirections[MAX_SCENE_LIGHTS];
uniform vec3  lightSpotCutOffs[MAX_SCENE_LIGHTS];
uniform float lightSpotExponents[MAX_SCENE_LIGHTS];


// ------------------------- Interpolated variables ---------------------------

in vec3 thePosition;
in vec3 theNormal;
in vec2 theTexCoord;

out vec4 fragmentColor;


// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // TODO: Implement your full Phong lighting here.
    // Available inputs are now in world-space:
    // - thePosition, theNormal
    // - cameraPosition
    // - light* arrays + lightCount
    vec3 color = vec3(0.0);

    color += ambientColor;

    color += matAmbient;
    color += matDiffuse;
    color += matSpecular * matShininess;

    fragmentColor = vec4(color, 1.0);
}
