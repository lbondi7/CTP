#version 450
#extension GL_ARB_separate_shader_objects : enable

//layout(binding = 2) uniform LightShit
//{
//    vec3 col;
//    float intensity;
//    vec3 direction;
//    float diffuseIntensity;
//    vec3 pos;
//    float constant;
//    float linear;
//    float exponent;
//    vec3 camPos;
//    int numberOfLights;
//} lights;

layout(std140, binding = 2) uniform LightShit
{
    vec3 col[10000000];
    float intensity[10000000];
    vec3 direction[10000000];
    float diffuseIntensity[10000000];
    vec3 pos[10000000];
    float constant[10000000];
    float linear[10000000];
    float exponent[10000000];
    vec3 camPos;
    int numberOfLights;
} lights;


layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 inPosition;
layout(location = 3) in vec4 inNormal;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;

vec4 CalcLightInternal(int i, vec3 LightDirection, vec3 Normal, vec4 pos)
{
    vec4 AmbientColor = vec4(lights.col[i], 1.0f) * lights.intensity[i];
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);
    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(lights.col[i] * lights.diffuseIntensity[i] * DiffuseFactor, 1.0f);

        // vec3 VertexToEye = normalize(light.camPos - pos.xyz);
        // vec3 LightReflect = normalize(reflect(light.direction, Normal));
        // float SpecularFactor = dot(VertexToEye, LightReflect);
        // if (SpecularFactor > 0) {
        //     SpecularFactor = pow(SpecularFactor, 32);
        //     SpecularColor = vec4(light.col * 1.0f * SpecularFactor, 1.0f);
        // }
    }

    return (AmbientColor + DiffuseColor);
}

// vec4 CalcPointLight(int i, vec3 Normal, vec4 pos)
// {
//      vec4 posWorld = pos * ubo.model;
//     vec3 LightDirection = posWorld.xyz - lights[i].pos;
//     float Distance = length(LightDirection);
//     LightDirection = normalize(LightDirection);

//     vec4 Color = CalcLightInternal(LightDirection, Normal.xyz, pos);
//     float Attenuation = lights[i].constant +
//                         lights[i].linear * Distance +
//                         lights[i].exponent * Distance * Distance;

//     return Color / Attenuation;
// }

void main() {
//
//    vec4 TotalLight = vec4(0, 0, 0, 0);// = CalcLightInternal(lights.direction, inNormal.xyz, inPosition);
//
//    for(int i = 0; i < lights.numberOfLights; ++i)
//    {
//    vec3 LightDirection = inPosition.xyz - lights.pos[i];
//    float Distance = length(LightDirection);
//    LightDirection = normalize(LightDirection);
//
//    vec4 Color = CalcLightInternal(i, LightDirection, inNormal.xyz, inPosition);
//    float Attenuation = lights.constant[i] +
//                        lights.linear[i] * Distance +
//                        lights.exponent[i] * Distance * Distance;
//
//    vec4 pointColour = Color / Attenuation;
//
//    TotalLight += pointColour;
//    }
  outColor = fragColor * texture(texSampler, fragTexCoord);
// outColor = fragColor;
}