#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inLPosition;
layout(location = 5) in vec4 inLColour;
layout(location = 6) in vec3 inLAtten;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 outLight;

vec4 CalcPointLight()
{
    vec3 LightDirection = gl_Position.xyz - inLPosition;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    vec4 Color = vec4(inLColour.xyz, 1.0f) * inLColour.w;
    float Attenuation = inLAtten.x +
                        inLAtten.y * Distance +
                        inLAtten.z * Distance * Distance;

    return Color / Attenuation;
}

void main() {
	vec4 pos = vec4(inPosition, 1.0);

	gl_Position = ubo.proj * ubo.view * ubo.model * pos;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    outLight = vec4(1, 1, 1, 1.0);
    //outLight = CalcPointLight();
}
