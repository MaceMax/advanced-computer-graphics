#version 410 core

out vec4 color;
in vec2 texCoord;

const int MaxNumberOfLights = 50; // 22x22

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

uniform bool isDebug; // If true, we will render one of the GBuffer textures.
                      // If false, we will render the final image.

// Debug textures
uniform sampler2D screenTexture;
uniform bool isDepth;

// Uniforms for final image
uniform LightSource lights[MaxNumberOfLights];
uniform vec3 viewPos;
uniform sampler2D gPositionAmbient; // xyz = position, w = ambient r value
uniform sampler2D gNormalAmbient; // xyz = normal, w = ambient g value
uniform sampler2D gAlbedoAmbient; // rgb = albedo, a = ambient b value
uniform sampler2D gAoMetallicRoughness; // x = ambient occlusion, yz metallic and roughness factors
                                        // If there is no metallic and roughness textures, y will contain specular factor and z will contain shininess factor
uniform sampler2D gDepth;

uniform sampler2DArray directionalShadowMaps;
uniform samplerCubeArray pointShadowMaps;

vec4 renderDebug() {
    if (isDepth) {
        float depth = texture(screenTexture, texCoord).r;
        return vec4(vec3(depth), 1.0);
    } else {
        vec3 texColor = texture(screenTexture, texCoord).rgb;
        return vec4(texColor, 1.0);
    }
}

vec3 calculatePointLight(LightSource light, vec3 fragPos, vec3 normal, vec3 albedo, vec3 viewDir, float ambientOcclusion, float metallic, float roughness) {
    vec3 lightDir = light.position.xyz - fragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    float attenuation = 1.0 / (1.0 + light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse.rgb * albedo * diff;

    // Specular shading
    vec3 specularColor;
    if (dot(normal, lightDir) < 0.0 || roughness == 0.0) {
        specularColor = vec3(0.0);
    } else {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), roughness); // TODO: Use roughness and metallic
        specularColor = light.specular.rgb * spec * metallic;
    }

    // Ambient occlusion. TODO: Implement this

    // Combine results
    vec3 result = (diffuseColor + specularColor) * attenuation;

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
  float bias = max(0.002 * (1.0 - dot(nNormal, lightDirection)), 0.0001);

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

vec3 calculateDirectionalLight(LightSource light, vec3 fragPos, vec3 normal, vec3 albedo, vec3 viewDir, float ambientOcclusion, float metallic, float roughness) {
    vec3 lightDir = normalize(light.position.xyz);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse.rgb * albedo * diff;
    // Specular shading
    vec3 specularColor;
    if (dot(normal, lightDir) < 0.0 || roughness == 0.0) {
        specularColor = vec3(0.0);
    } else {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), roughness); // TODO: Use roughness and metallic
        specularColor = light.specular.rgb * spec * metallic;
    }

    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0);
    float shadow = calculateDirectionalShadow(fragPosLightSpace, lightDir, normal, light.shadowMapIndex);
    // Combine results
    vec3 result =  (diffuseColor + specularColor);

    return result;
}

vec4 renderFinal() {
    // Retrieve data from GBuffer
    vec3 fragPos = texture(gPositionAmbient, texCoord).xyz;
    vec3 normal = texture(gNormalAmbient, texCoord).xyz;
    vec3 albedo = texture(gAlbedoAmbient, texCoord).xyz;
    float ambientOcclusion = texture(gAoMetallicRoughness, texCoord).x;
    float metallic = texture(gAoMetallicRoughness, texCoord).y; // If there is no metallic texture, y will contain specular factor
    float roughness = texture(gAoMetallicRoughness, texCoord).z; // If there is no roughness texture, z will contain shininess factor
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
                lighting += calculateDirectionalLight(lights[i], fragPos, normal, albedo, viewDir, ambientOcclusion, metallic, roughness);
                ambient = lights[i].ambient.rgb;
            } else {
                lighting += calculatePointLight(lights[i], fragPos, normal, albedo, viewDir, ambientOcclusion, metallic, roughness);
            }
        }
    }

    lighting += ambient * ambientColor;

    return vec4(lighting, 1.0);
}

void main() {
    if (isDebug) {
        color = renderDebug();
    } else {
        color = renderFinal();
    }
}


