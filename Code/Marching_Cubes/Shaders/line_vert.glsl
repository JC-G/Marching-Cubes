#version 330
precision highp float;

layout(location = 0) in vec4 vertexPosition_modelSpace;

uniform mat4 V;
uniform mat4 P;

void main() {
	vec4 vertexPosition_worldSpace = vertexPosition_modelSpace;
	gl_Position = P*V*vertexPosition_worldSpace;
}