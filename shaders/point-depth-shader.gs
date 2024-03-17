#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];
uniform int depthMapIndex;

out vec4 position;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = depthMapIndex * 6 + face;
        for(int i = 0; i < 3; ++i)
        {
            position = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * position;
            EmitVertex();
        }
        EndPrimitive();
    }
}