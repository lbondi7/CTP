#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformBufferParticle {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {

    mat4 model;

    for(int i = 0; i < 4; ++i)
    {
        model[i][i] = 1;
    }


   vec4 row0 = vec4(model[0][0], model[0][1], model[0][2], model[0][3]);
    vec4 row1 = vec4(model[1][0], model[1][1], model[1][2], model[1][3]);
    vec4 row2 = vec4(model[2][0], model[2][1], model[2][2], model[2][3]);
    vec4 row3 = vec4(model[3][0], model[3][1], model[3][2], model[3][3]);
    
    vec4 vector = row0 * 0.0f + row1 * 0.0f + row2 * 0.0f + row3;

    model[3][0] = vector.x;
        model[3][1] = vector.y;
            model[3][2] = vector.z;
                model[3][3] = vector.w;
    
	gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);

    fragColor = inColor;
    fragTexCoord = inTexCoord;
}