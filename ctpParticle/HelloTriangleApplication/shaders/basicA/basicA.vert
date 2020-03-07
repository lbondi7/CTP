#version 450
#extension GL_ARB_separate_shader_objects : enable
// #extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// layout(binding = 2) uniform LightShit {
//     vec3 col;
//     float intensity;
//     vec3 direction;
//     float diffuseIntensity;

// } light;

layout(binding = 2) uniform LightShit {
    vec3 col;
    float intensity;
    vec3 direction;
    float diffuseIntensity;
    vec3 pos;
    float constant;
    float linear;
    float exponent;
    vec3 camPos;
} lights[];

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord;
//layout(location = 4) in int inNumberofLights;
// layout(location = 4) in vec3 inLPosition;
// layout(location = 5) in vec4 inLColour;
// layout(location = 6) in vec3 inLAtten;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 outLight;


vec4 CalcLightInternal(vec3 LightDirection, vec3 Normal, vec4 pos)
{
    vec4 AmbientColor = vec4(lights[0].col, 1.0f) * lights[0].intensity;
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);
    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(lights[0].col * lights[0].diffuseIntensity * DiffuseFactor, 1.0f);

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

	vec4 pos = vec4(inPosition, 1.0);

    //vec4 AmbientColor = vec4(light.col * light.intensity, 1.0);

    vec4 Normal = vec4(inNormal, 0.0) * ubo.model;

    vec4 TotalLight = CalcLightInternal(lights[0].direction, Normal.xyz, pos);

    vec4 posWorld = pos * ubo.model;
    vec3 LightDirection = posWorld.xyz - lights[0].pos;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    vec4 Color = CalcLightInternal(LightDirection, Normal.xyz, posWorld);
    float Attenuation = lights[0].constant +
                        lights[0].linear * Distance +
                        lights[0].exponent * Distance * Distance;

    vec4 pointColour = Color / Attenuation;

    TotalLight += pointColour;

    LightDirection = posWorld.xyz - lights[1].pos;
    Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    Color = CalcLightInternal(LightDirection, Normal.xyz, posWorld);
    Attenuation = lights[1].constant +
                        lights[1].linear * Distance +
                        lights[1].exponent * Distance * Distance;

    pointColour = Color / Attenuation;

    TotalLight += pointColour;

    // for(int i = 0; i < 2; ++i)
    // {
    //     TotalLight += CalcPointLight(i, Normal.xyz, pos);
    // }

	gl_Position = ubo.proj * ubo.view * ubo.model * pos;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    outLight = TotalLight;
    //outLight = CalcPointLight();
}
