#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout (location = 3) in vec3 instancePos;
layout (location = 4) in vec3 instanceRot;
layout (location = 5) in float instanceScale;
layout (location = 6) in vec4 instanceColor;
layout (location = 7) in int instanceTexIndex;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
	vec4 pos = vec4((inPosition.xyz * instanceScale) + instancePos, 1.0);

	gl_Position = ubo.proj * ubo.view * ubo.model * pos;

    fragColor = instanceColor * inColor;
    fragTexCoord = inTexCoord;
}