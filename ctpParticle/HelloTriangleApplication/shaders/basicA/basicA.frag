#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Light
{
   vec3 col;
   vec3 pos;
};

layout(std140, binding = 2) buffer Lights
{
   Light lights[];
};

layout(binding = 3) uniform uboLight
{
   vec3 cameraPos;
   int particleCount;
} lightConsts;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragPosition;
layout(location = 3) in vec4 fragNormal;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;

// vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
// {
//     vec3 lightDir = normalize(-light.direction);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     // combine results
//     vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
//     vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
//     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//     return (ambient + diffuse + specular);
// }  

vec4 CalcDirLight(vec3 dirLightDirection, vec3 normal, vec3 pos)
{
   vec3 lightDir = normalize(-dirLightDirection);

   float diff = max(dot(normal, lightDir), 0.0);

   vec3 reflectDir = reflect(-lightDir, normal);

   vec3 viewDir = normalize(pos - lightConsts.cameraPos);

   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

   float diffCol = 0.2;
   float specCol = 0.5;

   vec4 ambient = vec4(0.5, 0.5, 0.5, 0.0) * 0.0;
   vec4 diffuse = vec4(diffCol, diffCol, diffCol, 0.0) * diff;
   vec4 specular = vec4(specCol, specCol, specCol, 0.0) * spec;

   return ambient + diffuse + specular;
}

vec4 CalPointLight(int i, vec3 pointLightDir, vec3 Normal, vec3 pos)
{

    float DiffuseFactor = dot(Normal, pointLightDir);

    float diffCol = 0.0;
    float specCol = 1.0;
    vec4 DiffuseColor = vec4(diffCol, diffCol, diffCol, 1.0);
    vec4 SpecularColor = vec4(lights[i].col, 1.0);
    if (DiffuseFactor > 0) {
        DiffuseColor *= DiffuseFactor;

        vec3 VertexToEye = normalize(pos - lightConsts.cameraPos);
        vec3 LightReflect = normalize(reflect(pointLightDir, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        if (SpecularFactor > 0.0) {
            SpecularFactor = pow(SpecularFactor, 128);
            SpecularColor *= SpecularFactor;
        }
    }
    return (DiffuseColor + SpecularColor);
}


// vec3 CalPointLight(int i, vec3 normal, vec3 fragPos, vec3 viewDir)
// {
//     vec3 lightDir = normalize(lights[i].pos - fragPos);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//     // attenuation
//     float distance    = length(lights[i].pos - fragPos);
//     float attenuation = 1.0 / (1.0 + 0.09 * distance + 
//   			     0.032 * (distance * distance));    
//     // combine results
//     vec3 ambient  = lights[i].col;
//     vec3 diffuse  = lights[i].col * diff;
//     vec3 specular = lights[i].col * spec;
//     ambient  *= attenuation;
//     diffuse  *= attenuation;
//     specular *= attenuation;
//     return (ambient + diffuse + specular);
// }

void main() {

   vec4 TotalLight = CalcDirLight(vec3(0, -0.1, 1), fragNormal.xyz, fragPosition.xyz);

   for(int j = 0; j < lightConsts.particleCount; j++)
   {
   vec3 pointLightDir = lights[j].pos - fragPosition.xyz;
   float Distance = length(pointLightDir);
   pointLightDir = normalize(-pointLightDir);

   //vec3 Color = CalPointLight(j,fragNormal.xyz, fragPosition.xyz, lightConsts.cameraPos);
   
   vec4 Color = CalPointLight(j, pointLightDir, fragNormal.xyz, fragPosition.xyz);
   
   float Attenuation = (1.0 +
                       (0.9 * Distance) +
                       (0.032 * (Distance * Distance)));

   TotalLight += Color / Attenuation;
//   TotalLight += vec4(Color, 0.0f);
   }
  
  outColor = fragColor * texture(texSampler, fragTexCoord) * TotalLight;
}