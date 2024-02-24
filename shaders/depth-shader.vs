#version 410 core

layout(location = 0) in vec4 vertex_position;

uniform mat4 m, lsm;

void main() {   
    gl_Position = lsm * m * vertex_position;
}


