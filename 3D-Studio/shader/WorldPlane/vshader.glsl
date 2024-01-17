#version 330
layout (location = 0) in vec3 vPos;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

void main()
{
    vec4 pos = P * V * M * vec4(vPos, 1.0);
    gl_Position = pos;
}
