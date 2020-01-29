#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

layout(location = 0) in vec4 inColor[];
layout(location = 1) in vec2 inTexCoord[];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {

    float size = 0.5;
    vec4 offset = vec4(-size, size, 0, 1);
    vec4 vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = vertexPos;
    fragColor = inColor[0];
    fragTexCoord = vec2(0, 0);
    EmitVertex();

    offset = vec4(size, -size, 0, 1);
    vertexPos = gl_in[0].gl_Position+ offset;
    gl_Position = vertexPos;
    fragColor = inColor[0];
    fragTexCoord = vec2(1, 1);
    EmitVertex();

    offset = vec4(-size, -size, 0, 1);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = vertexPos;
    fragColor = inColor[0];
    fragTexCoord = vec2(0, 1);
    EmitVertex();

    EndPrimitive();

    offset = vec4(-size, size, 0, 1);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = vertexPos;
    fragColor = inColor[0];
    fragTexCoord = vec2(0, 0);
    EmitVertex();

    offset = vec4(size, size, 0, 1);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = vertexPos;
    fragColor = inColor[0];
    fragTexCoord = vec2(1, 0);
    EmitVertex();

    offset = vec4(size, -size, 0, 1);
    vertexPos = gl_in[0].gl_Position + offset;
    gl_Position = vertexPos;
    fragColor = inColor[0];
    fragTexCoord = vec2(1, 1);
    EmitVertex();

    EndPrimitive();
}