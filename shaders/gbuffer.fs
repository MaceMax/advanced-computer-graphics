#version 410 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;

in vec4 position;
in vec3 normal;
in vec3 texCoord;

const int MAX_MATERIAL_TEXTURES=8;

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

// The front surface material
uniform Material material;
uniform Textures textureLayers;

void main()
{
    gPosition = position.xyz;

    gNormal = normalize(normal);
    if (material.activeTextures[3])
    {
        vec3 norm = texture(material.textures[3], texCoord).rgb;
        norm = norm * 2.0 - 1.0;
        gNormal = normalize(TBN * norm);
    }

    gAlbedoSpec = material.diffuse.rgb;
    if (material.activeTextures[0])
    {
        gAlbedoSpec = texture(material.textures[0], texCoord).rgb;
    }

    if (material.activeTextures[1])
    {
        gAlbedoSpec.a = texture(material.textures[1], texCoord).r;
    }
}
