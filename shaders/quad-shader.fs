#version 410 core

out vec4 color;

in vec2 texCoord;

uniform sampler2D screenTexture;

void main() {
    color = texture(screenTexture, texCoord);
}
