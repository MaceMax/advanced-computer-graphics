#version 330

/*
* wireframe shader
*/

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec4 vColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


out vec3 normal;
out vec3 fragPos;
out vec4 color;
out vec2 texCoords;

void main()
{
    color = vColor;
    gl_Position=P*V*M*vec4(vPos, 1.0);
}