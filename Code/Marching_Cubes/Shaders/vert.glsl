//basic, do-nothing vertex shader
#version 330
precision highp float;

layout(location = 0) in vec4 vertexPosition_modelSpace;
layout(location = 1) in vec4 vertexNormal_modelSpace;

out vec4 vertexPosition_worldSpace;
out vec4 vertexNormal_worldSpace;

uniform mat4 V;
uniform mat4 P;

void main() {
	vertexPosition_worldSpace = vertexPosition_modelSpace;
	vertexNormal_worldSpace = vertexNormal_modelSpace;
	gl_Position = P*V*vertexPosition_worldSpace;
}
