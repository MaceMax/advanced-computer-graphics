#version 410 core

out float color;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];
uniform mat4 p;
uniform mat4 v;
uniform int kernelSize;

uniform float screenX;
uniform float screenY;
uniform float farPlane;

float radius = 0.5;
float bias = 0.025;

void main() {
    vec2 noiseScale = vec2(screenX / 4.0, screenY / 4.0);

    vec3 fragPos = texture(gPosition, texCoord).xyz;
    vec3 normal = texture(gNormal, texCoord).xyz;
    vec3 randomVec = texture(noiseTexture, texCoord * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float ssao = 0.0;
    for (int i = 0; i < kernelSize; ++i) {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = p * v * vec4(samplePos, 1.0);
        offset.xyz /= offset.w; // perspective division
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        float sampleDepth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        
        ssao += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    ssao = 1.0 - ssao / kernelSize;
    color = ssao;
}