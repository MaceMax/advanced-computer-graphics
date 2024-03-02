#version 410 core

out vec4 color;

in vec2 texCoord;

uniform sampler2D screenTexture;
uniform bool isDepth;

void main() {
    if (isDepth) {
        float depth = texture(screenTexture, texCoord).r;
        color = vec4(vec3(depth), 1.0);
    } else {
        vec3 texColor = texture(screenTexture, texCoord).rgb;
        color = vec4(texColor, 1.0);
    }
    
}
