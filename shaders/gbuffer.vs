#version 410 core

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texCoord;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitangent;

out vec4 position;  // position of the vertex (and fragment) in world space
out vec3 normal;  // surface normal vector in world space
out vec2 texCoord;  // texture coordinates
out mat3 TBN;  // TBN matrix 

uniform mat4 m, v, p;  // model, view, and projection matrices
uniform mat3 m_3x3_inv_transp; // Inverse transpose of model matrix for transforming normals

void main()
{
    vec4 world_position = m * vertex_position;
    position = v * world_position; // should i multiply by view matrix?
    texCoord = vertex_texCoord;

    normal = m_3x3_inv_transp * vertex_normal;

    vec3 T = normalize(vec3(m * vec4(vertex_tangent, 0.0)));
    vec3 B = normalize(vec3(m * vec4(vertex_bitangent, 0.0)));
    vec3 N = normalize(vec3(m * vec4(vertex_normal, 0.0)));

    TBN = mat3(T, B, N);

    gl_Position = p * position;
}