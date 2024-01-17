#version 330
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 P;
uniform mat4 V;

void main()
{
    texCoords = aPos;
    vec4 pos = P * V * vec4(aPos, 0);
    gl_Position = pos.xyww;
}
