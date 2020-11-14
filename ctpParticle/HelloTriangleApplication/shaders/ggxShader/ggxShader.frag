#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Light
{
	vec4 pos;
   vec4 vel;
   vec4 col;
   bool goToTri;
   float randDirDur;
   int targetTri;
   vec4 targetPoint;
};

layout(std140, binding = 2) readonly buffer Lights
{
   Light lights[];
};

layout(binding = 3) uniform uboLight
{
   vec3 cameraPos;
   int lightCount;
   int frameNum;
} lightConsts;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPosition;
layout(location = 3) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;


// vec3 CalPointLight(Light pointLight, vec3 normal, vec3 viewDir)
// {

//    vec3 lightDir = normalize(fragPosition - pointLight.pos.xyz);

//    float theta = max(dot(normal, -lightDir), 0.0);
//    float theta2 = max(dot(normal, -viewDir), 0.0);
//    vec3 light = vec3(0.2);

//    light = ((viewDir * theta) / (lightDir * theta)) * light;
//    return light;
// }


////////////////////////////////////////////////////////////////////////////////////////////

///////// This code was externally sourced and can be found in the link below //////////////
///////// https://graphics.rwth-aachen.de:9000/Glow/glow-extras/blob/cfce9b4c4f5b806e4981c0ccad7de0e46edac68c/material/shader/glow-material/material-ggx.glsl //////////

////////////////////////////////////////////////////////////////////////////////////////////

// see http://www.filmicworlds.com/2014/04/21/optimizing-ggx-shaders-with-dotlh/
vec3 shadingSpecularGGX(vec3 N, vec3 V, vec3 L, float roughness, vec3 F0)
{
    vec3 H = normalize(V + L);

    float dotLH = max(dot(L, H), 0.0);
    float dotNH = max(dot(N, H), 0.0);
    float dotNL = max(dot(N, L), 0.0);
    float dotNV = max(dot(N, V), 0.0);

    float alpha = roughness * roughness;

    // D (GGX normal distribution)
    float alphaSqr = alpha * alpha;
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (denom * denom);
    // no pi because BRDF -> lighting

    // F (Fresnel term)
    float F_a = 1.0;
    float F_b = pow(1.0 - dotLH, 5); // manually?
    vec3 F = mix(vec3(F_b), vec3(F_a), F0);

    // G (remapped hotness, see Unreal Shading)
    float k = (alpha + 2 * roughness + 1) / 8.0;
    float G = dotNL / (mix(dotNL, 1, k) * mix(dotNV, 1, k));

    return D * F * G / 4.0;
}

// specular and diffuse contribution of a single light direction
vec3 shadingGGX(vec3 N, vec3 V, vec3 L, vec3 color, float roughness, float metallic)
{
    vec3 diffuse = color * (1 - metallic); // metals have no diffuse
    vec3 specular = mix(vec3(0.04), color, metallic); // fixed spec for non-metals

    float dotNL = max(dot(N, L), 0.0);

    return diffuse * dotNL + shadingSpecularGGX(N, V, L, roughness, specular);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

bool isInside(vec3 pos, vec3 min, vec3 max)
{
    if((pos.x <= max.x && pos.x >= min.x) &&
    (pos.y <= max.y && pos.y >= min.y) &&
    (pos.z <= max.z && pos.z >= min.z))
        return true;

    return false;
}

void getLights(inout Light l[1])
{
   vec3 vMin = vec3(1.0 / 0.0);
   vec3 vMax = vec3(-(1.0 / 0.0));
   for(int i = 0; i < lightConsts.lightCount; ++i)
   {
      if(lights[i].col.x == -1)
      {
         continue;
      }

      vMin.x = min(vMin.x, lights[i].pos.x);
      vMin.y = min(vMin.y, lights[i].pos.y);
      vMin.z = min(vMin.z, lights[i].pos.z);

      vMax.x = max(vMax.x, lights[i].pos.x);
      vMax.y = max(vMax.y, lights[i].pos.y);
      vMax.z = max(vMax.z, lights[i].pos.z);  
   }


   vec3 voxelSize = (vMax - vMin) / 1;
   int numOfLights[1];

   vec3 bound = vMin;
   for(int i = 0; i < 1; ++i)
   {
      for(int j = 0; j < lightConsts.lightCount; ++i)
      {
         if(isInside(lights[j].pos.xyz, bound, bound + voxelSize)){
            l[i].pos += lights[j].pos;
            l[i].col += lights[j].col;
            ++numOfLights[i];
         }
      }
   }

   for(int i = 0; i < 1; ++i)
   {
      for(int j = 0; j < lightConsts.lightCount; ++i)
      {
         if(isInside(lights[j].pos.xyz, bound, bound + voxelSize)){
            l[i].pos /= numOfLights[i];
            l[i].col /= numOfLights[i];
         }
      }
   }


}

void main() {
  
  //if(lightConsts.frameNum == 1)
  //{

   // Light l[1];
   // getLights(l);

   vec3 normal = normalize(fragNormal);
   vec3 viewVec = fragPosition - lightConsts.cameraPos;
   vec3 viewDir = normalize(fragPosition - lightConsts.cameraPos);
   vec3 invViewDir = normalize(lightConsts.cameraPos - fragPosition);
   vec3 totalLight = vec3(0);

   for(int i = 0; i < lightConsts.lightCount; ++i)
   {
      if(lights[i].col.x == -1)
         continue;
         
      vec3 lightVec = fragPosition - lights[i].pos.xyz;
      vec3 lightDir = normalize(fragPosition - lights[i].pos.xyz);
      vec3 invLightDir = normalize(lights[i].pos.xyz - fragPosition);
      totalLight += (shadingGGX(normal, invViewDir, invLightDir, lights[i].col.xyz, 0.01, 0.99) * lights[i].col.w);
   }

   // for(int i = 0; i < 1; ++i)
   // {
   //    if(l[i].col.x == -1)
   //       continue;
         
   //    vec3 lightVec = fragPosition - l[i].pos.xyz;
   //    vec3 lightDir = normalize(fragPosition - l[i].pos.xyz);
   //    vec3 invLightDir = normalize(l[i].pos.xyz - fragPosition);
   //    totalLight += (shadingGGX(normal, invViewDir, invLightDir, l[i].col.xyz, 0.01, 0.99) * l[i].col.w);
   // }

  outColor = vec4(totalLight, 1.0);

  //outColor = fragColor * texture(texSampler, fragTexCoord) * vec4(totalLight, 1.0);
  //}
  //outColor = vec4(1, 1, 1, 1);
}