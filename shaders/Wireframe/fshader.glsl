#version 330

/*
* wireframe shader
*/

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 normal;
in vec4 color;
in vec3 fragPos;
in vec2 texCoords;
out vec4  fColor;

//uniform vec3 lightPos;

uniform vec3 ambientLightColor;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform Material material;
uniform int hasTexture;
uniform sampler2D texture0;


void main()
{
    fColor = color;
}
