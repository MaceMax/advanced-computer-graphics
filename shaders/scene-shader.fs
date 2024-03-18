#version 410 core

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 brightColor;

in vec2 texCoord;

const int MaxNumberOfLights = 50; // 22x22

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

struct LightSource
{
  bool enabled;
  vec4 position;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;

  float constant;
  float linear;
  float quadratic;
  float farPlane;
  int shadowMapIndex; // Index of the shadow map in the array. If it is a point light, it will be the index of the first face of the cubemap.
  mat4 lightSpaceMatrix; // Used for shadow mapping
};


// Uniforms for final image
uniform LightSource lights[MaxNumberOfLights];
uniform vec3 viewPos;
uniform bool shadowsEnabled;
uniform sampler2D gPositionAmbient; // xyz = position, w = ambient r value
uniform sampler2D gNormalAmbient; // xyz = normal, w = ambient g value
uniform sampler2D gAlbedoAmbient; // rgb = albedo, a = ambient b value
uniform sampler2D gAoMetallicRoughness; // x = ambient occlusion, yz metallic and roughness factors
                                        // If there is no metallic and roughness textures, y will contain specular factor and z will contain shininess factor

uniform sampler2DArray directionalShadowMaps;
uniform samplerCubeArray pointShadowMaps;

float calculatePointShadow(vec3 fragPos, vec3 lightPos, vec3 viewDir, float farPlane,int index) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    // PCF
    float shadow = 0.0;
    float bias = 0.20;
    int samples = 10;
    float viewDistance = length(fragPos - viewPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

    for (int i = 0; i < samples; ++i)
    {
        vec3 direction = normalize(fragToLight + sampleOffsetDirections[i] * diskRadius);
        vec4 texCoords = vec4(direction, float(index));
        float closestDepth = texture(pointShadowMaps, texCoords).r;
        closestDepth *= farPlane;  // Undo mapping [0;1]
        if (currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
            
    }

  return shadow / float(samples);
}

vec3 calculatePointLight(LightSource light, vec3 fragPos, vec3 normal, vec3 albedo, vec3 viewDir, float ambientOcclusion, float metallic, float shininess) {
    vec3 lightDir = light.position.xyz - fragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    float attenuation = 1.0 / (1.0 + light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse.rgb * albedo * diff;

    // Specular shading
    vec3 specularColor;
    if (dot(normal, lightDir) < 0.0 || shininess == 0.0) {
        specularColor = vec3(0.0);
    } else {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess); // TODO: Use roughness and metallic
        specularColor = light.specular.rgb * spec * metallic;
    }

    float shadow = 0.0;
    if (shadowsEnabled) 
        shadow = calculatePointShadow(fragPos, light.position.xyz, viewDir, light.farPlane, light.shadowMapIndex);

    // Combine results
    vec3 result = (1.0 - shadow) * ((diffuseColor * 2 + specularColor) * attenuation);

    return result;
}

float calculateDirectionalShadow(vec4 fragPosLightSpace, vec3 lightDirection, vec3 nNormal, int index)
{
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  projCoords = projCoords * 0.5 + 0.5;
 
  if (projCoords.z > 1.0) {
    return 0.0;
  }

  float shadow = 0.0;
  float currentDepth = projCoords.z;
  float bias = max(0.002 * (1.0 - dot(nNormal, lightDirection)), 0.001);

  vec2 texelSize = 1.0 / textureSize(directionalShadowMaps, 0).xy;

  int kernelSize = 10;
  for(int x = -kernelSize; x <= kernelSize; ++x)
  {
    for(int y = -kernelSize; y <= kernelSize; ++y)
    {
      float pcfDepth = texture(directionalShadowMaps, vec3(projCoords.xy + vec2(x, y) * texelSize, index)).r; 
      shadow += currentDepth - bias > pcfDepth  ? 0.95 : 0.0;        
    }
  }
  
  return shadow / ((2 * kernelSize + 1) * (2 * kernelSize + 1));
}

vec3 calculateDirectionalLight(LightSource light, vec3 fragPos, vec3 normal, vec3 albedo, vec3 viewDir, float ambientOcclusion, float metallic, float shininess) {
    vec3 lightDir = normalize(light.position.xyz);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse.rgb * albedo * diff;
    // Specular shading
    vec3 specularColor;
    if (dot(normal, lightDir) < 0.0 || shininess == 0.0) {
        specularColor = vec3(0.0);
    } else {
        specularColor = vec3(light.specular) * vec3(metallic)
                * pow(max(0.0, dot(reflect(-lightDir, normal), viewDir)), 32); // TODO: Use roughness and metallic
    }

    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0);
    float shadow = 0.0;
    if (shadowsEnabled) 
        shadow = calculateDirectionalShadow(fragPosLightSpace, lightDir, normal, light.shadowMapIndex);
    
    
    // Combine results
    vec3 result =  (1.0 - shadow) * (diffuseColor + specularColor);

    return result;
}

vec4 renderFinal() {
    // Retrieve data from GBuffer
    vec3 fragPos = texture(gPositionAmbient, texCoord).xyz;
    vec3 normal = texture(gNormalAmbient, texCoord).xyz;
    vec3 albedo = texture(gAlbedoAmbient, texCoord).xyz;
    float ambientOcclusion = texture(gAoMetallicRoughness, texCoord).x;
    float metallic = texture(gAoMetallicRoughness, texCoord).y; // If there is no metallic texture, y will contain specular factor
    float shininess = texture(gAoMetallicRoughness, texCoord).z; // If there is no roughness texture, z will contain shininess factor
    vec3 ambientColor = vec3(texture(gPositionAmbient, texCoord).w,
                                texture(gNormalAmbient, texCoord).w,
                                texture(gAlbedoAmbient, texCoord).w);

    // Calculate lighting
    vec3 lighting = vec3(0.0);
    vec3 ambient = vec3(0.0);
    vec3 viewDir = normalize(viewPos - fragPos);
    for (int i = 0; i < MaxNumberOfLights; i++) {
        if (lights[i].enabled) {
            if (lights[i].position.w == 0.0) {
                lighting += calculateDirectionalLight(lights[i], fragPos, normal, albedo, viewDir, ambientOcclusion, metallic, shininess);
                ambient = lights[i].ambient.rgb;
            } else {
                lighting += calculatePointLight(lights[i], fragPos, normal, albedo, viewDir, ambientOcclusion, metallic, shininess);
            }
        }
    }
    
    lighting += ambient * ambientColor;

    if (albedo == vec3(0.0)) {
        lighting = vec3(0.0);
    }

    return vec4(lighting, 1.0);
}

void main() {
    color = renderFinal();

    // Brightness
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    float lower = 0.9; // start of bloom
    float upper = 1.0; // end of bloom
    float factor = smoothstep(lower, upper, brightness);
    brightColor = vec4(color.rgb * factor, 1.0);
}


