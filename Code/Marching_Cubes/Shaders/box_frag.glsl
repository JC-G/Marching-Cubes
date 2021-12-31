#version 330 core
in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;

void main() {
    color = texture(renderedTexture,UV);
}