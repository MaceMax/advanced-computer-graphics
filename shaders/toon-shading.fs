#version 410 core

// From vertex shader
in vec4 position;  // position of the vertex (and fragment) in eye space
in vec3 normal ;  // surface normal vector in eye space
in vec2 texCoord; // Texture coordinate

// The end result of this shader
out vec4 color;

uniform mat4 m, v, p;
uniform mat4 v_inv;
uniform int numberOfLights;
uniform int lightingEnabled;

const int MAX_MATERIAL_TEXTURES=3;
const int MAX_TEXTURES=10;

// declaration of a Material structure
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    bool activeTextures[MAX_MATERIAL_TEXTURES];
    sampler2D textures[MAX_MATERIAL_TEXTURES];
};

// Definition of a light source structure
struct LightSource
{
  bool enabled;
  vec4 position;
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

// Some hard coded default ambient lighting
vec4 scene_ambient = vec4(0.2, 0.2, 0.2, 1.0);

// The front surface material
uniform Material material;
uniform Textures textureLayers;

// Toon parameters
int toonLevels = 3;
float toonScaleFactor = 1.0 / toonLevels;

void main()
{
  vec3 normalDirection = normalize(normal);
  vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
  vec3 lightDirection;
  float attenuation;

  // initialize total lighting with ambient lighting
  vec3 totalLighting = vec3(scene_ambient) * vec3(material.ambient);


  
  if (lightingEnabled == 1) {
      // for all light sources
      for (int index = 0; index < numberOfLights; index++) 
      {
        LightSource light = lights[index];
        if (0.0 == light.position.w) // directional light?
        {
          attenuation = 1.0; // no attenuation
          lightDirection = normalize(vec3(light.position));
        }
        else // point light or spotlight (or other kind of light) 
        {
          vec3 positionToLightSource = vec3(light.position - position);
          float distance = length(positionToLightSource);
          lightDirection = normalize(positionToLightSource);
          attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        }

        // Toon calculations
        float diffFactor = dot(normalDirection, lightDirection);

        if (diffFactor > 0) {
          diffFactor = ceil(diffFactor * toonLevels) * toonScaleFactor;        
        }

        vec3 diffuseReflection = attenuation
          * vec3(light.diffuse) * vec3(material.diffuse)
          * max(0.0, dot(normalDirection, lightDirection)) * diffFactor;
        
        // This is a temporary solution, since if i don't use these, they will be optimized away and glUniformLocation will return -1
        vec3 specularReflection = vec3(material.specular) * viewDirection * material.shininess;
        totalLighting = totalLighting + diffuseReflection + (specularReflection * 0.0000001);
      }
  }

  // How we could check for a diffuse texture map
  if (material.activeTextures[0])
  {
    vec4 diffuseTex = texture2D(material.textures[0], texCoord);
    totalLighting = totalLighting * diffuseTex.rgb;
  }

  vec4 blendedColor = vec4(0, 0, 0, 1);
  int activeTextureCount = 0;

  for (int index = 0; index < MAX_TEXTURES; index++) {
    if (textureLayers.activeTextures[index]) {
      vec4 layerColor = texture(textureLayers.textures[index], texCoord);
      blendedColor = mix(blendedColor, layerColor, 1.0 / (activeTextureCount + 1.0));
      activeTextureCount += 1;
    }
  }
  
  if (activeTextureCount > 0) {
    totalLighting = totalLighting * blendedColor.rgb;  
  }

  color = vec4(totalLighting, 1.0);
}
