#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

out vec2 UV;
uniform vec2 screenSize;

uniform vec2 position;
uniform vec2 size;

void main() {
    vec4 res = vec4(vertexPosition_modelspace.xy,0,1);
    //transform quad in [-1,1] to quad in [position, position+size]
    //TODO - transformations exist for a reason...
    //res.xy = size * (res.xy - vec2(1.0))/vec2(2.0) + position;

    res.xy = (size * (vec2(1.0) + res.xy) + 2 * position)/screenSize - vec2(1.0);
    gl_Position = res;
    UV = (vertexPosition_modelspace.xy+vec2(1))/2;
}