#version 410 core
layout (location = 0) out vec4 gPositionEmissive; // xyz = position, w = emissive r value
layout (location = 1) out vec4 gNormalEmissive; // xyz = normal, w = emissive g value
layout (location = 2) out vec4 gAlbedoEmissive; // rgb = albedo, a = emissive b value
layout (location = 3) out vec4 gAoMetallicRoughness; // x = ambient occlusion, yz metallic and roughness factors, w = specular factor

in vec4 position;
in vec3 normal;
in vec2 texCoord;
in mat3 TBN;

const int MAX_MATERIAL_TEXTURES=8;
const int MAX_TEXTURES=2;

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

struct Textures
{
  bool activeTextures[MAX_TEXTURES];
  sampler2D textures[MAX_TEXTURES];
};

// The front surface material
uniform Material material;
uniform Textures textureLayers;

void main()
{
    gPositionEmissive.xyz = position.xyz;

    gNormalEmissive.xyz = normalize(normal);
    if (material.activeTextures[3]) // Normal map
    {
        vec3 norm = texture(material.textures[3], texCoord).rgb;
        norm = norm * 2.0 - 1.0;
        gNormalEmissive.xyz = normalize(TBN * norm);
    }

    gAlbedoEmissive.rgb = material.diffuse.rgb * material.ambient.rgb;
    if (material.activeTextures[0]) // Diffuse map
    {
        gAlbedoEmissive.rgb = texture(material.textures[0], texCoord).rgb * material.ambient.rgb;
    }

    if (material.activeTextures[6]) // Metallic 
    {
        gAoMetallicRoughness.y = texture(material.textures[6], texCoord).r;
    } else if (material.activeTextures[1]) // Specular map
    {
        vec3 specularColor = texture(material.textures[1], texCoord).rgb;
        float specularGray = dot(specularColor, vec3(0.299, 0.587, 0.114)); // Convert to grayscale
        gAoMetallicRoughness.y = specularGray; // Use grayscale value as metallic factor
    } else { // No metallic or specular map, use specular color
        gAoMetallicRoughness.y = material.specular.r;
    }

    if (material.activeTextures[7]) // Roughness
    {
        gAoMetallicRoughness.z = texture(material.textures[7], texCoord).r;
    } else {
        // Compute roughness from shininess
        // See: https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
        gAoMetallicRoughness.z = sqrt(2.0 / (material.shininess + 2.0)); 
    }

    if (material.activeTextures[4]) // Ambient occlusion
    {
        gAoMetallicRoughness.x = texture(material.textures[4], texCoord).r;
    } else {
        gAoMetallicRoughness.x = 0.0;
    }

    if (material.activeTextures[5]) { // Emissive map
        vec3 emissive = texture(material.textures[5], texCoord).rgb;
        gPositionEmissive.w = emissive.r;
        gNormalEmissive.w = emissive.g;
        gAlbedoEmissive.w = emissive.b;
    } else {
        gPositionEmissive.w = material.emission.r;
        gNormalEmissive.w = material.emission.g;
        gAlbedoEmissive.w = material.emission.b;
    }

}
