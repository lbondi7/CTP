#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Light
{
   vec4 col;
   vec4 pos;
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

// const float PI = 3.14159265359;

// vec3 getNormalFromMap()
// {
//     vec3 tangentNormal = vec3(0.96, 0.96, 0.97) * 2.0 - 1.0;

//     vec3 Q1  = dFdx(fragPosition);
//     vec3 Q2  = dFdy(fragPosition);
//     vec2 st1 = dFdx(fragTexCoord);
//     vec2 st2 = dFdy(fragTexCoord);

//     vec3 N   = normalize(fragNormal);
//     vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
//     vec3 B  = -normalize(cross(N, T));
//     mat3 TBN = mat3(T, B, N);

//     return normalize(TBN * tangentNormal);
// }

// float DistributionGGX(vec3 N, vec3 H, float roughness)
// {
//     float a = roughness*roughness;
//     float a2 = a*a;
//     float NdotH = max(dot(N, H), 0.0);
//     float NdotH2 = NdotH*NdotH;

//     float nom   = a2;
//     float denom = (NdotH2 * (a2 - 1.0) + 1.0);
//     denom = PI * denom * denom;

//     return nom / denom;
// }
// // ----------------------------------------------------------------------------
// float GeometrySchlickGGX(float NdotV, float roughness)
// {
//     float r = (roughness + 1.0);
//     float k = (r*r) / 8.0;

//     float nom   = NdotV;
//     float denom = NdotV * (1.0 - k) + k;

//     return nom / denom;
// }
// // ----------------------------------------------------------------------------
// float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
// {
//     float NdotV = max(dot(N, V), 0.0);
//     float NdotL = max(dot(N, L), 0.0);
//     float ggx2 = GeometrySchlickGGX(NdotV, roughness);
//     float ggx1 = GeometrySchlickGGX(NdotL, roughness);

//     return ggx1 * ggx2;
// }
// // ----------------------------------------------------------------------------
// vec3 fresnelSchlick(float cosTheta, vec3 F0)
// {
//     return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
// }

// void main()
// {		
//     vec3 albedo     = vec3(0.96, 0.96, 0.97);
//     float metallic  = 0.91;
//     float roughness = 0.91;
//     float ao        = 0.91;

//     vec3 N = vec3(0.96, 0.96, 0.97);
//     vec3 V = normalize(lightConsts.cameraPos - fragPosition);

//     // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
//     // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
//     vec3 F0 = vec3(0.04); 
//     F0 = mix(F0, albedo, metallic);

//     // reflectance equation
//     vec3 Lo = vec3(0.0);
//     for(int i = 0; i < lightConsts.particleCount; ++i) 
//     {
//         // calculate per-light radiance
//         vec3 L = normalize(lights[i].pos.xyz - fragPosition);
//         vec3 H = normalize(V + L);
//         float distance = length(lights[i].pos.xyz - fragPosition);
//         float attenuation = 1.0 / (distance * distance);
//         vec3 radiance = lights[i].col.xyz * attenuation;

//         // Cook-Torrance BRDF
//         float NDF = DistributionGGX(N, H, roughness);   
//         float G   = GeometrySmith(N, V, L, roughness);      
//         vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
//         vec3 nominator    = NDF * G * F; 
//         float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
//         vec3 specular = nominator / denominator;
        
//         // kS is equal to Fresnel
//         vec3 kS = F;
//         // for energy conservation, the diffuse and specular light can't
//         // be above 1.0 (unless the surface emits light); to preserve this
//         // relationship the diffuse component (kD) should equal 1.0 - kS.
//         vec3 kD = vec3(1.0) - kS;
//         // multiply kD by the inverse metalness such that only non-metals 
//         // have diffuse lighting, or a linear blend if partly metal (pure metals
//         // have no diffuse light).
//         kD *= 1.0 - metallic;	  

//         // scale light by NdotL
//         float NdotL = max(dot(N, L), 0.0);        

//         // add to outgoing radiance Lo
//         Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
//     }   
    
//     // ambient lighting (note that the next IBL tutorial will replace 
//     // this ambient lighting with environment lighting).
//     vec3 ambient = vec3(0.03) * albedo * ao;
    
//     vec3 color = ambient + Lo;

//     // HDR tonemapping
//     color = color / (color + vec3(1.0));
//     // gamma correct
//     color = pow(color, vec3(1.0/2.2)); 

//     outColor = vec4(color, 1.0);
// }


vec3 CalcDirLight(vec3 dirLightDirection, vec3 normal, vec3 pos)
{
   vec3 lightDir = normalize(-dirLightDirection);

   float diff = max(dot(normal, lightDir), 0.0);

   vec3 reflectDir = reflect(-lightDir, normal);

   vec3 viewDir = normalize(pos - lightConsts.cameraPos);

   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

   float ambCol = 0.05;
   float diffCol = 0.1;
   float specCol = 0.5;

   vec3 ambient = vec3(ambCol) * vec3(texture(texSampler, fragTexCoord));
   vec3 diffuse = vec3(diffCol) * vec3(texture(texSampler, fragTexCoord)) * diff;
   vec3 specular = vec3(specCol) * vec3(texture(texSampler, fragTexCoord)) * spec;

   return ambient + diffuse + specular;
}

// vec3 CalPointLight(Light light, vec3 normal, vec3 pos)
// {
//    vec3 pointLightDir = normalize(light.pos.xyz - fragPosition);
//    vec3 viewDir = normalize(pos - lightConsts.cameraPos);
//    vec3 refLightDir = reflect(-pointLightDir, normal);
//    float diffFactor = max(dot(normal, pointLightDir), 0.0);
//    float specFactor = pow(max(dot(viewDir, refLightDir), 0.0), 16.0);

//    float distance = length(light.pos.xyz - pos);
//    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));  

//    float ambCol = 0.05;
//    float diffCol = 0.8;
//    float specCol = 1.0;
//    vec3 ambient = vec3(ambCol) * vec3(texture(texSampler, fragTexCoord));
//    vec3 diffuse = vec3(diffCol) * vec3(texture(texSampler, fragTexCoord)) * diffFactor;
//    vec3 specular = vec3(specCol) * vec3(texture(texSampler, fragTexCoord)) * specFactor;

//    ambient  *= attenuation;
//    diffuse  *= attenuation;
//    specular *= attenuation;

//    return (ambient + diffuse + specular);
// }

vec3 CalPointLight(Light light, vec3 normal, vec3 fragPosition, vec3 viewDir)
{
   // vec3 lightDir = normalize(light.pos.xyz - fragPosition);
   //  // diffuse shading
   //  float diff = max(dot(normal, lightDir), 0.0);
   //  // specular shading
   //  vec3 reflectDir = reflect(-lightDir, normal);
   //  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
   //  // attenuation
   //  float distance = length(light.pos.xyz - fragPosition);
   //  float attenuation = 1.0 / (1.0 + 0.5 * distance + 2.5 * (distance * distance));    
   //  // combine results
   //  vec3 ambient = light.col.xyz * vec3(0.05) * vec3(texture(texSampler, fragTexCoord));
   //  vec3 diffuse = light.col.xyz * vec3(0.8) * diff * vec3(texture(texSampler, fragTexCoord));
   //  vec3 specular = light.col.xyz * vec3(1.0) * spec * vec3(texture(texSampler, fragTexCoord));
   //  ambient *= attenuation;
   //  diffuse *= attenuation;
   //  specular *= attenuation;
   // return (ambient + diffuse + specular);

   vec3 albedo = vec3(texture(texSampler, fragTexCoord));
   // Now hide latency of texture lookup
   vec3 lightDir = light.pos.xyz - fragPosition;
   float distance = length(lightDir);
   lightDir = normalize(lightDir);
   //Attenuation
   float G = max(dot(normal, lightDir), 0.0) / (distance * distance);
   // Diffuse bit of the BRDF
   vec3 diffuse = (albedo / 3.141592654);
   // Phong
   vec3 reflectDir = reflect(-lightDir, normal);
   float phong = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

   // Put it all together: intensity * albedo * (Diffuse_weight / pi + (1-Diffuse_weight)*phong) * G
   // This assumes that there is no specular colour
   float diffuse_weight = 0.5;
   vec3 direct = light.col.xyz * (albedo * ((diffuse_weight / 3.141592654f) + (1.0f - diffuse_weight) * phong)) * G;
   // Questionable ambient term
   vec3 ambient = albedo * vec3(0.2, 0.2, 0.2);
   return (direct + ambient);
}

void main() {

   vec3 norm = normalize(fragNormal);
   vec3 viewDir = normalize(lightConsts.cameraPos - fragPosition);
   vec3 TotalLight = CalcDirLight(vec3(-0.2, -1.0, -0.3), norm, fragPosition);

  // vec3 TotalLight = vec3(0);

   for(int i = 0; i < lightConsts.particleCount; ++i)
   {
   // vec3 pointLightDir = normalize(fragPosition - lights[i].pos.xyz);


   // vec3 colour = CalPointLight(lights[i], norm, fragPosition);
      vec3 colour = CalPointLight(lights[i], norm, fragPosition, viewDir);
   TotalLight += colour;
   }
  
  outColor = vec4(TotalLight, 1.0);
  //outColor = fragColor * texture(texSampler, fragTexCoord) * vec4(TotalLight, 1.0);
}