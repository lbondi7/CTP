#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformBufferParticle {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inVelocity;
layout(location = 2) in float inAlpha;

layout(location = 0) out float outAlpha;
//layout(location = 0) out vec4 fragColor;

void main() {    

    vec4 pos = inPosition;
    pos.w = 1.0;
	gl_Position = ubo.proj * ubo.view * ubo.model * pos;
    outAlpha = inAlpha;
}