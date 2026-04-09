#version 140

// -------------------------------- Macros ------------------------------------

// light types for better distinction
#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTION_LIGHT 2

#define MAX_SCENE_LIGHTS 16

// ------------------------------- Uniforms -----------------------------------

uniform float elapsedTime;      // elapsed application time
uniform vec3  ambientColor;     // ambient color
uniform vec3  cameraPosition;   // position of the camera

uniform mat4  PVM;              // Projection * View * Model -> model to clip coords
uniform mat4  viewMatrix;       // view matrix
uniform mat4  modelMatrix;      // model matrix
uniform mat4  normalMatrix;     // inverse transposed model matrix

uniform sampler2D diffuseTex;

// current material parameters
uniform vec3  matDiffuse;       // diffuse parameter of the material
uniform vec3  matSpecular;      // specular parameter of the material
uniform vec3  matAmbient;       // ambient parameter of the material
uniform float matShininess;     // shininess parameter of the material

// all scene lights parameters
uniform int   lightCount;
uniform int   lightTypes[MAX_SCENE_LIGHTS];
uniform vec3  lightAmbients[MAX_SCENE_LIGHTS];
uniform vec3  lightDiffuses[MAX_SCENE_LIGHTS];
uniform vec3  lightSpeculars[MAX_SCENE_LIGHTS];
uniform vec3  lightPositions[MAX_SCENE_LIGHTS];
uniform vec3  lightSpotDirections[MAX_SCENE_LIGHTS];
uniform float lightSpotCutOffs[MAX_SCENE_LIGHTS];
uniform float lightSpotExponents[MAX_SCENE_LIGHTS];
uniform float lightIntensities[MAX_SCENE_LIGHTS];


// ------------------------------- Attributes ---------------------------------

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 theTexCoord;

out vec4 fragmentColor;

// ------------------------------- Functions ----------------------------------

vec3 getColorFromLight(vec3 albedo, vec3 position, vec3 normal, int index) {
    vec3 lightVec = lightPositions[index] - position;
    float lightDist = length(lightVec);
    float lightFallof = 1.0 / (lightDist * lightDist);

    vec3 L = normalize(lightVec);

    // oprava directional light
    if (lightTypes[index] == DIRECTION_LIGHT) {
        L = normalize(-lightSpotDirections[index]);
        lightFallof = 1.0;
    }

    vec3 R = reflect(-L, normal);
    vec3 V = normalize(cameraPosition - position);

    float NdotL = max(dot(normal, L), 0.0);
    float spotCos = dot(normalize(lightSpotDirections[index]), -L);
    float spotEffect = pow(max(spotCos, 0.0), lightSpotExponents[index]);
    float cosB = max(dot(R, V), 0.0);

    vec3 ambient = lightAmbients[index] * matAmbient * albedo;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (NdotL <= 0.0) return ambient;

    // calculating light based on the light type
    switch (lightTypes[index]) {
        // point light calculation
        case POINT_LIGHT:
            diffuse = NdotL *
                      lightDiffuses[index] *
                      matDiffuse *
                      albedo *
                      lightFallof;
            specular = pow(cosB, matShininess) *
                       lightSpeculars[index] *
                       matSpecular *
                       lightFallof;
            break;

        // spot light calculation
        case SPOT_LIGHT:
            if (spotCos < lightSpotCutOffs[index])
                return ambient;

            diffuse = NdotL *
                      spotEffect *
                      lightDiffuses[index] *
                      matDiffuse *
                      albedo *
                      lightFallof;
            specular = spotEffect *
                       pow(cosB, matShininess) *
                       lightSpeculars[index] *
                       matSpecular *
                       lightFallof;
            break;

        // directional light calculation
        case DIRECTION_LIGHT:
            diffuse = NdotL *
                      lightDiffuses[index] *
                      matDiffuse *
                      albedo;
            specular = pow(cosB, matShininess) *
                       lightSpeculars[index] *
                       matSpecular;
            break;
    }

    return ambient + lightIntensities[index] * (diffuse + specular);
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // world-coordinates position and normal of vertex
    vec3 position = worldPosition;
    vec3 normal = normalize(worldNormal);

    // diffuse texture color
    vec3 albedo = texture(diffuseTex, theTexCoord).rgb;

    vec3 color = ambientColor * matAmbient * albedo;

    for (int i = 0; i < lightCount; i++) {
        color += getColorFromLight(albedo, position, normal, i);
    }

    fragmentColor = vec4(color, 1.0);
}
