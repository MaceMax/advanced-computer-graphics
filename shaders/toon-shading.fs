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

// Toon parameters
int toonLevels = 3;
float toonScaleFactor = 1.0 / toonLevels;

void main()
{
  vec3 normalDirection = normalize(normal);
  vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
  vec3 lightDirection;
  float attenuation;

  vec3 totalLighting = vec3(0.0, 0.0, 0.0);

  
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
            }
            else // point light or spotlight (or other kind of light) 
            {
              vec4 positionWorld = v_inv * position;
              vec3 positionToLightSource = vec3(light.position - positionWorld);
              float distance = length(positionToLightSource);
              lightDirection = normalize(positionToLightSource);
              attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
            }
            
            vec3 ambientReflection = attenuation * vec3(light.ambient) * vec3(material.ambient); 
            // Toon calculations
            float diffFactor = dot(normalDirection, lightDirection);

            if (diffFactor > 0) {
              diffFactor = ceil(diffFactor * toonLevels) * toonScaleFactor;        
            }

            vec3 diffuseReflection = attenuation
              * vec3(light.diffuse) * vec3(material.diffuse)
              * max(0.0, dot(normalDirection, lightDirection)) * diffFactor;

            vec3 specularReflection;
            if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
            {
              specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
            }
            else // light source on the right side
            {
              specularReflection = attenuation * vec3(light.specular) * vec3(material.specular)
                * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
            }
            totalLighting += ambientReflection + diffuseReflection + specularReflection;
        }       
        
      }
  }

  // How we could check for a diffuse texture map
  vec4 diffuseTex = vec4(0.0, 0.0, 0.0, 1.0);
  if (material.activeTextures[0])
  {
    diffuseTex = texture2D(material.textures[0], texCoord);
    totalLighting = totalLighting * diffuseTex.rgb;
  }
  float alpha = diffuseTex.a;
  
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
