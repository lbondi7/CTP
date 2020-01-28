#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (points) in;

layout (triangle_strip, max_vertices = 4) out;

layout(location = 0) in vec4 inColor[];
layout(location = 1) in vec2 inTexCoord[];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {

    vec4 offset = vec4(-1, 1, 0, 0);
    vec4 vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = ubo.proj * ubo.view * ubo.model * vertexPos;
    fragColor = inColor[0];
    fragTexCoord = inTexCoord[0];
    EmitVertex();

    offset = vec4(1, -1, 0, 0);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = ubo.proj * ubo.view * ubo.model * vertexPos;
    fragColor = inColor[0];
    fragTexCoord = inTexCoord[0];
    EmitVertex();

    offset = vec4(-1, -1, 0, 0);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = ubo.proj * ubo.view * ubo.model * vertexPos;
    fragColor = inColor[0];
    fragTexCoord = inTexCoord[0];
    EmitVertex();

    EndPrimitive();

    offset = vec4(-1, 1, 0, 0);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = ubo.proj * ubo.view * ubo.model * vertexPos;
    fragColor = inColor[0];
    fragTexCoord = inTexCoord[0];
    EmitVertex();

    offset = vec4(1, 1, 0, 0);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = ubo.proj * ubo.view * ubo.model * vertexPos;
    fragColor = inColor[0];
    fragTexCoord = inTexCoord[0];
    EmitVertex();

    offset = vec4(1, -1, 0, 0);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = ubo.proj * ubo.view * ubo.model * vertexPos;
    fragColor = inColor[0];
    fragTexCoord = inTexCoord[0];
    EmitVertex();

    EndPrimitive();
}