#version 410 core

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texCoord;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitangent;

const int MaxNumberOfLights = 10;

out vec4 position;  // position of the vertex (and fragment) in world space
out vec3 normal;  // surface normal vector in world space
out vec2 texCoord;
out mat3 TBN;  // TBN matrix 
out vec4 positionLightSpace[MaxNumberOfLights];


// model, view and projection transform
uniform mat4 m, v, p;
uniform bool activeLights[MaxNumberOfLights];
uniform mat4 lightSpaceMatrix[MaxNumberOfLights];

// Inverse transpose of model matrix for transforming normals
uniform mat3 m_3x3_inv_transp;

void main()
{
  mat4 mv = v * m;
  texCoord = vertex_texCoord;

  position = mv * vertex_position;
  normal = normalize(m_3x3_inv_transp * vertex_normal);

  vec3 T = normalize(vec3(m * vec4(vertex_tangent, 0.0)));
  vec3 B = normalize(vec3(m * vec4(vertex_bitangent, 0.0)));
  vec3 N = normalize(vec3(m * vec4(vertex_normal, 0.0)));

  for (int i = 0; i < MaxNumberOfLights; i++)
  {
    if (activeLights[i])
    {
      positionLightSpace[i] = lightSpaceMatrix[i] * vertex_position;
    }
  }

  TBN = mat3(T, B, N);

  gl_Position = p * position;
}
