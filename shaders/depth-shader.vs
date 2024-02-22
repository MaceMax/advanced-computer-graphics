#version 410 core

layout(location = 0) in vec4 vertex_position;

uniform mat4 m, v, p;

void main() {   
    gl_Position = p * v * m * vertex_position;
}


