#version 410 core

out vec4 color;

in vec2 texCoord;

uniform sampler2D screenTexture;

void main() {
    vec3 texColor = texture(screenTexture, texCoord).rgb;
    color = vec4(texColor, 1.0);
}
