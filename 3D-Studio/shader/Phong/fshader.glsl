#version 330

/*
* Phong lighting model
*/

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
    
    float cutOff;
    float outerCutOff;
};

in vec3 normal;
in vec4 color;
in vec3 fragPos;
in vec2 texCoords;
out vec4  fColor;

uniform Light light;
uniform vec3 viewPos;
uniform Material material;
uniform int hasTexture;
uniform sampler2D texture0;

void main()
{   
    vec3 norm = normalize(normal);

    vec3 lightDir;
    if (light.position.w == 0.0) {
        lightDir = normalize(light.position.xyz);
    } else {
        lightDir = normalize(light.position.xyz - fragPos);
    }
    
    vec3 ambient;
    if (material.hasAmbientMap == 1) {
        ambient = light.ambient * vec3(texture(material.ambientMap, texCoords));
    } else if (material.hasDiffuseMap == 1) {
        ambient = light.ambient * vec3(texture(material.diffuseMap, texCoords));
    } else {
        ambient = light.ambient * material.ambient;
    }

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse;
    vec4 diffuseColor;
    if (material.hasDiffuseMap == 1) {
        diffuseColor = texture(material.diffuseMap, texCoords);
        diffuse = light.diffuse * diff * vec3(diffuseColor);
    } else {
        diffuseColor = vec4(material.diffuse, 1.0);
        diffuse = light.diffuse * (diff * material.diffuse);
    }

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specular;
    if (material.hasSpecularMap == 1) {
        specular = light.specular * spec * vec3(texture(material.specularMap, texCoords));
    } else {
        specular = light.specular * (spec * material.specular);
    }

    if (light.type == 1) {
        float distance = length(light.position.xyz - fragPos);
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

    if (hasTexture == 1) {
        fColor = texture(texture0, texCoords) * result * color;
    } else {
        fColor = result * color;
    }
}
