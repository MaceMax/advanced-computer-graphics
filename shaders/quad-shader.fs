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

const float gamma = 0.8;
const float exposure = 3.0;

void main() {
    if (isDepth) {
        float depth = texture(screenTexture, texCoord).r;
        color = vec4(vec3(depth), 1.0);
    } else {
        vec3 screenColor = texture(screenTexture, texCoord).rgb;

        if (bloomEnabled) {
            vec3 bloomColor = texture(bloomTexture, texCoord).rgb;
            screenColor += texture(bloomTexture, texCoord).rgb;

            screenColor += bloomColor;

            vec3 result = vec3(1.0) - exp(-screenColor * exposure); // tone mapping
            screenColor = pow(result, vec3(1.0 / gamma)); // gamma correction
        }

        if (dofEnabled) {
            vec3 focusPoint = texture(depthTexture, texCoord).rgb;
            float depth = focusPoint.r;
            float dist = abs(depth - focusDistance);
            float blur = 1.0 - smoothstep(0.0, 1.0, dist / (aperture * 2));
            screenColor = mix(screenColor, texture(blurTexture, texCoord).rgb, blur);

        }

        color = vec4(screenColor, 1.0);
    }
    
}