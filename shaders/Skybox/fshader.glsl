#version 330

in vec3 texCoords;
out vec4 fColor;

uniform samplerCube skybox;

void main()
{
    fColor = texture(skybox, texCoords);
}