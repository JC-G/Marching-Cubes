#version 330 core
layout(location = 0) in vec4 vertexPosition_modelspace;
uniform mat4 transform;
uniform mat4 V;
uniform mat4 P;

void main() {
    gl_Position = P*V*transform * vertexPosition_modelspace;
}