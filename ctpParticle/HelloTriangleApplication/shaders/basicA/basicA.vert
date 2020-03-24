#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord;
// layout(location = 4) in vec3 inLPosition;
// layout(location = 5) in vec4 inLColour;
// layout(location = 6) in vec3 inLAtten;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragPosition;
layout(location = 3) out vec4 fragNormal;

void main() {

	vec4 pos = vec4(inPosition, 1.0);

	gl_Position = ubo.proj * ubo.view * ubo.model * pos;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragPosition = vec4(inPosition, 1.0) * ubo.model;
    fragNormal = vec4(inNormal, 1.0) * mat4(transpose(inverse(ubo.model)));
}
