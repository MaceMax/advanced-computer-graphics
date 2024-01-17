#version 330

/*
* Gouraud lighting model
*/

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec3 vColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

struct Material
{
    int hasAmbientMap;
    int hasDiffuseMap;
    int hasSpecularMap;
    int hasNormalMap;

    sampler2D ambientMap;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    int type; // 0 == directional, 1 == point, 2 == spot
    vec4 position; // w == 0.0 means it's a direction vector, not a position
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform vec3 viewPos;
uniform Material material;
uniform int hasTexture;
uniform sampler2D texture0;

out vec4 color;
out vec2 texCoords;

void main()
{
    vec3 tNormal = mat3(transpose(inverse(M))) * vNormal;
    vec3 norm = normalize(tNormal);
    vec3 lightDir;
    if (light.position.w == 0.0) {
        lightDir = normalize(light.position.xyz);
    } else {
        lightDir = normalize(light.position.xyz - vPos);
    }
    
    vec3 ambient;
    if (material.hasAmbientMap == 1) {
        ambient = light.ambient * vec3(texture(material.ambientMap, vTexCoords));
    } else if (material.hasDiffuseMap == 1) {
        ambient = light.ambient * vec3(texture(material.diffuseMap, vTexCoords));
    } else {
        ambient = light.ambient * material.ambient;
    }

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse;
    vec4 diffuseColor;
    if (material.hasDiffuseMap == 1) {
        diffuseColor = texture(material.diffuseMap, vTexCoords);
        diffuse = light.diffuse * diff * vec3(diffuseColor);
    } else {
        diffuseColor = vec4(material.diffuse, 1.0);
        diffuse = light.diffuse * (diff * material.diffuse);
    }

    vec3 viewDir = normalize(viewPos - vPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specular;
    if (material.hasSpecularMap == 1) {
        specular = light.specular * spec * vec3(texture(material.specularMap, vTexCoords));
    } else {
        specular = light.specular * (spec * material.specular);
    }

    if (light.type == 1) {
        float distance = length(light.position.xyz - vPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
    }

    vec4 result;
    if (dot(norm, lightDir) < 0.0)
    {
        result = vec4(ambient + diffuse, diffuseColor.a);
    }
    else
    {
        result = vec4(ambient + diffuse + specular, diffuseColor.a);
    }

    color = result * vec4(vColor, 1.0);
    texCoords = vTexCoords;
    gl_Position=P*V*M*vec4(vPos, 1.0);
}