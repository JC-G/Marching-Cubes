#version 330

precision highp float;
layout(location = 0) out vec4 color;

const vec4 lineColor = vec4(1,0,0,1);

void main() {
    color = lineColor;
}
