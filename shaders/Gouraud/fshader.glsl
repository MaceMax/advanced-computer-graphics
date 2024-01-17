#version 330

/*
* Gouraud lighting model
*/

in vec4 color;
in vec2 texCoords;
out vec4  fColor;

uniform int hasTexture;
uniform sampler2D texture0;

void main()
{
    if (hasTexture == 1) {
        fColor = texture(texture0, texCoords) * color;
    } else {
        fColor = color;
    }
}
