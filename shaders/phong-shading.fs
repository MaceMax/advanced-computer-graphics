#version 410 core

// From vertex shader
in vec4 position;  // position of the vertex (and fragment) in eye space
in vec3 normal;  // surface normal vector in eye space
in vec2 texCoord; // Texture coordinate
in mat3 TBN; // Tangent, bitangent and normal matrix

// The end result of this shader
out vec4 color;

uniform mat4 m, v, p;
uniform mat4 v_inv;
uniform int numberOfLights;
uniform int lightingEnabled;

const int MAX_MATERIAL_TEXTURES=8;
const int MAX_TEXTURES=10;

// declaration of a Material structure
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;

    float shininess;
    bool activeTextures[MAX_MATERIAL_TEXTURES];
    sampler2D textures[MAX_MATERIAL_TEXTURES];
};

// Definition of a light source structure
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
};

struct Textures
{
  bool activeTextures[MAX_TEXTURES];
  sampler2D textures[MAX_TEXTURES];
};
 
const int MaxNumberOfLights = 10;

// This is the uniforms that our program communicates with
uniform LightSource lights[MaxNumberOfLights];

// The front surface material
uniform Material material;
uniform Textures textureLayers;

void main()
{
  vec3 wNormal = normalize(normal);
  // Check for normal map
  if (material.activeTextures[3]) {
    wNormal = texture(material.textures[3], texCoord).rgb;
    wNormal = wNormal * 2.0 - 1.0;
    wNormal = normalize(TBN * wNormal);
  }  


  vec3 normalDirection = wNormal;
  vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
  vec3 lightDirection;
  float attenuation;
  vec3 totalLighting = vec3(0.0, 0.0, 0.0);
  
  vec4 diffuseColor = material.diffuse;
  vec4 specularColor = material.specular;
  vec4 ambientColor = material.ambient;
  vec4 emissionColor = material.emission;

  if (material.activeTextures[0])
  {
    diffuseColor = texture(material.textures[0], texCoord);
  }
  float alpha = diffuseColor.a;

  if (material.activeTextures[1])
  {
      specularColor = texture(material.textures[1], texCoord);
  }

  if (material.activeTextures[5])
  {
      emissionColor = texture(material.textures[5], texCoord);
  }
  
  vec3 ambientReflection = vec3(0.0, 0.0, 0.0);
  if (lightingEnabled == 1) {
      // for all light sources
      for (int index = 0; index < numberOfLights; index++) 
      {
        LightSource light = lights[index];
        if (light.enabled) {
            if (0.0 == light.position.w) // directional light?
            {
              attenuation = 1.0; // no attenuation
              lightDirection = normalize(vec3(light.position));
              ambientReflection = vec3(light.ambient); // A scene should not have more than one directional light
            }
            else // point light or spotlight (or other kind of light) 
            {
              vec4 positionWorld = v_inv * position;
              vec3 positionToLightSource = vec3(light.position - positionWorld);
              float distance = length(positionToLightSource);
              lightDirection = normalize(positionToLightSource);
              attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
            }
            
        
            vec3 diffuseReflection = attenuation
              * vec3(light.diffuse) * vec3(diffuseColor)
              * max(0.0, dot(normalDirection, lightDirection));

            vec3 specularReflection;
            if (material.shininess == 0.0 || dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
            {
              specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
            }
            else // light source on the right side
            {
              specularReflection = attenuation * vec3(light.specular) * vec3(specularColor)
                * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
            }
            totalLighting += diffuseReflection + specularReflection;
        }       
        
      }
  }

  ambientReflection = ambientReflection * vec3(ambientColor) * vec3(material.ambient);
  totalLighting = totalLighting + ambientReflection + vec3(emissionColor);

  vec4 blendedColor;
  int activeTextureCount = 0;

  for (int index = 0; index < MAX_TEXTURES; index++) {
    if (textureLayers.activeTextures[index]) {
      if (activeTextureCount == 0) {
        blendedColor = texture(textureLayers.textures[index], texCoord);
      } else {
        vec4 layerColor = texture(textureLayers.textures[index], texCoord);
        blendedColor = blendedColor * layerColor;
      }
      activeTextureCount += 1;
    }
  }
  
  if (activeTextureCount > 0) {
    totalLighting = totalLighting * blendedColor.rgb;
    alpha = alpha * blendedColor.a;  
  }
  
  color = vec4(totalLighting, alpha);
}
