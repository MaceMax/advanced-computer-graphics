#version 410 core

out vec4 color;
in vec2 texCoord;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform sampler2D depthTexture;
uniform sampler2D blurTexture;

uniform bool bloomEnabled;
uniform bool dofEnabled;
uniform bool isDepth;

uniform float focusDistance;
uniform float aperture;
uniform float near;
uniform float far;

const float gamma = 0.6;
const float exposure = 2.8;

void main() {
    if (isDepth) {
        float depth = texture(screenTexture, texCoord).r;
        color = vec4(vec3(depth), 1.0);
    } else {
        vec3 screenColor = texture(screenTexture, texCoord).rgb;

        if (dofEnabled) {
            float focusRange = (1.0 / (aperture * 2)) * 0.001; // 0.01 is just to make the effect more visible
            float depth = texture(depthTexture, texCoord).r;
            float linearDepth = near / (far - depth * (far - near));
            float dist = abs(focusDistance - linearDepth);
            float blurAmount = clamp(dist / focusRange, 0.0, 1.0);
            screenColor = mix(screenColor, texture(blurTexture, texCoord).rgb, blurAmount);
        }

        if (bloomEnabled) {
            vec3 bloomColor = texture(bloomTexture, texCoord).rgb;
            screenColor += texture(bloomTexture, texCoord).rgb;   
        }

        vec3 result = vec3(1.0) - exp(-screenColor * exposure); // tone mapping
        screenColor = pow(result, vec3(1.0 / gamma)); // gamma correction
        
        color = vec4(screenColor, 1.0);
    }
    
}