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
layout(location = 2) in vec3 fragPosition;
layout(location = 3) in vec3 fragNormal;

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

// vec4 CalcDirLight(vec3 dirLightDirection, vec3 normal, vec3 pos)
// {
//    vec3 lightDir = normalize(-dirLightDirection);

//    float diff = max(dot(normal, lightDir), 0.0);

//    vec3 reflectDir = reflect(-lightDir, normal);

//    vec3 viewDir = normalize(pos - lightConsts.cameraPos);

//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

//    float diffCol = 0.1;
//    float specCol = 0.5;

//    vec4 ambient = vec4(0.05, 0.05, 0.05, 0.0);
//    vec4 diffuse = vec4(diffCol, diffCol, diffCol, 0.0) * diff;
//    vec4 specular = vec4(specCol, specCol, specCol, 0.0) * spec;

//    return ambient + diffuse + specular;
// }


vec3 CalcDirLight(vec3 dirLightDirection, vec3 normal, vec3 pos)
{
   vec3 lightDir = normalize(-dirLightDirection);

   float diff = max(dot(normal, lightDir), 0.0);

   vec3 reflectDir = reflect(-lightDir, normal);

   vec3 viewDir = normalize(pos - lightConsts.cameraPos);

   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);

   float ambCol = 0.05;
   float diffCol = 0.1;
   float specCol = 0.5;

   vec3 ambient = vec3(ambCol) * vec3(texture(texSampler, fragTexCoord));
   vec3 diffuse = vec3(diffCol) * vec3(texture(texSampler, fragTexCoord)) * diff;
   vec3 specular = vec3(specCol) * vec3(texture(texSampler, fragTexCoord)) * spec;

   return ambient + diffuse + specular;
}

vec3 CalPointLight(int i, vec3 pointLightDir, vec3 normal, vec3 pos)
{
   vec3 viewDir = normalize(pos - lightConsts.cameraPos);
   vec3 refLightDir = reflect(-pointLightDir, normal);
   float diffFactor = max(dot(normal, pointLightDir), 0.0);
   float specFactor = pow(max(dot(viewDir, refLightDir), 0.0), 64.0);

   float distance    = length(lights[i].pos - pos);
   float attenuation = 1.0 / 
      (1.0 + 0.09 * distance + 
  			     0.032 * (distance * distance));  

   float ambCol = 0.05;
   float diffCol = 0.4;
   float specCol = 0.5;
   vec3 ambient = vec3(ambCol)* vec3(texture(texSampler, fragTexCoord));
   vec3 diffuse = vec3(diffCol) * vec3(texture(texSampler, fragTexCoord)) * diffFactor;
   vec3 specular = vec3(specCol) * vec3(texture(texSampler, fragTexCoord)) * specFactor;

   ambient  *= attenuation;
   diffuse  *= attenuation;
   specular *= attenuation;

   return (ambient + diffuse + specular);
}

// vec4 CalPointLight(int i, vec3 pointLightDir, vec3 normal, vec3 pos)
// {

//     float diffFactor = dot(normal, pointLightDir);

//     float diffCol = 0.0;
//     float specCol = 0.5;
//     vec4 ambient = vec4(diffCol, diffCol, diffCol, 1.0);
//     vec4 diffuse = vec4(diffCol, diffCol, diffCol, 1.0);
//     vec4 specular = vec4(specCol, specCol, specCol, 1.0);
//     if (diffFactor > 0) {
//         diffuse *= vec4(lights[i].col, 0.0) * diffFactor;

//         vec3 viewDir = normalize(pos - lightConsts.cameraPos);
//         vec3 refLightDir = normalize(reflect(pointLightDir, normal));
//         float specFactor = dot(viewDir, refLightDir);
//         if (specFactor > 0.0) {
//             specFactor = pow(specFactor, 128);
//             specular *= vec4(lights[i].col, 0.0) * specFactor;
//         }
//     }
//     return (ambient + diffuse + specular);
// }


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

   vec3 norm = normalize(fragNormal);

   vec3 TotalLight = CalcDirLight(vec3(-0.2, -1.0, -0.3), norm, fragPosition);

   for(int j = 0; j < lightConsts.particleCount; j++)
   {
   vec3 pointLightDir = lights[j].pos - fragPosition;
   float Distance = length(pointLightDir);
   pointLightDir = normalize(-pointLightDir);

   TotalLight += CalPointLight(j, pointLightDir, norm, fragPosition);

   //vec3 Color = CalPointLight(j,fragNormal.xyz, fragPosition.xyz, lightConsts.cameraPos);
   
   // vec4 Color = CalPointLight(j, pointLightDir, fragNormal.xyz, fragPosition.xyz);
   
   // float Attenuation = (1.0 +
   //                     (0.9 * Distance) +
   //                     (0.032 * (Distance * Distance)));

   // TotalLight += Color / Attenuation;
//   TotalLight += vec4(Color, 0.0f);
   }
  
  outColor = fragColor * vec4(TotalLight, 1.0);
  //outColor = fragColor * texture(texSampler, fragTexCoord) * vec4(TotalLight, 1.0);
}