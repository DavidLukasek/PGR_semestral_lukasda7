#version 140

// light types for better distinction
#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTION_LIGHT 2

#define MAX_SCENE_LIGHTS 16

// ------------------------------- Uniforms -----------------------------------

uniform float elapsedTime;      // elapsed application time
uniform vec3  ambientColor;     // ambient color
uniform vec3  cameraPosition;   // position of the camera

uniform vec3  matDiffuse;
uniform vec3  matSpecular;
uniform vec3  matAmbient;
uniform float matShininess;

uniform mat4  PVMmatrix;        // Projection * View * Model  --> model to clip coordinates
uniform mat4  viewMatrix;       // view matrix
uniform mat4  modelMatrix;      // model matrix
uniform mat4  normalMatrix;     // inverse transposed model matrix

// light-related uniforms
uniform int   lightTypes[MAX_SCENE_LIGHTS];
uniform vec3  lightAmbients[MAX_SCENE_LIGHTS];
uniform vec3  lightDiffuses[MAX_SCENE_LIGHTS];
uniform vec3  lightSpeculars[MAX_SCENE_LIGHTS];
uniform vec3  lightPositions[MAX_SCENE_LIGHTS];
uniform vec3  lightSpotDirections[MAX_SCENE_LIGHTS];
uniform float lightSpotCutOffs[MAX_SCENE_LIGHTS];
uniform float lightSpotExponents[MAX_SCENE_LIGHTS];


// ------------------------- Interpolated variables ---------------------------

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 theTexCoord;

out vec4 fragmentColor;

vec3 getColorFromLight(vec3 position, vec3 normal, int index) {
    vec3 ret = vec3(0.0);

    vec3 lightVec = lightPositions[index] - position;
    float lightDist = length(lightVec);
    float lightFallof = 1.0 / (lightDist*lightDist);

    vec3 L = normalize(lightVec);
    vec3 R = reflect(-L, normal);
    vec3 V = normalize(cameraPosition - position);

    float NdotL = max(dot(normal, L), 0.0);
    float spotCos = dot(normalize(lightSpotDirections[index]), -L);
    float spotEffect = pow(max(spotCos, 0.0), lightSpotExponents[index]);
    float cosB = max(dot(R, V), 0.0);

    vec3 ambient = lightAmbients[index] * matAmbient;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (NdotL <= 0.0) return vec3(0.0);

    // calculating light based on the light type
    switch (lightTypes[index]) {
        case POINT_LIGHT:
            diffuse = NdotL *
                      lightDiffuses[index] *
                      matDiffuse;
            specular = pow(cosB, matShininess) *
                       lightSpeculars[index] *
                       matSpecular;
            break;
        case SPOT_LIGHT:
            diffuse = NdotL *
                      spotEffect *
                      lightDiffuses[index] *
                      matDiffuse;
            specular = spotEffect *
                       pow(cosB, matShininess) *
                       lightSpeculars[index] *
                       matSpecular;

            if (spotCos < lightSpotCutOffs[index]) {
                diffuse = vec3(0.0);
                specular = vec3(0.0);
            }
            break;
        case DIRECTION_LIGHT:
            break;
    }

    ret += ambient + lightFallof*(diffuse + specular);

    return ret;
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // world-coordinates position and normal of vertex
    vec3 position = worldPosition;
    vec3 normal = normalize(worldNormal);

    // initialize color with ambient light
    vec3 color = ambientColor;

    for (int i = 0; i < MAX_SCENE_LIGHTS; i++) {
        color += getColorFromLight(position, normal, i);
    }

    // color = worldNormal;

    fragmentColor = vec4(color, 1.0);
}
