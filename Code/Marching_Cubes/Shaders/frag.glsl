//basic test fragment shader
#version 330

precision highp float;
layout(location = 0) out vec4 color;

in vec4 vertexPosition_worldSpace;
in vec4 vertexNormal_worldSpace;

const vec4 lightPos = vec4(0,100000,0,1);
const vec4 lightColor = vec4(1);
const float ambientPower = .10;
const float diffusePower = 1.0;
const float specularPower = 10.0;
const float specularExponent = 10.0;
const vec3 terrainColor = vec3(0.1,0.5,0.1);

uniform vec3 cameraPosition;

void main() {
    //ambient
    float ambientTotal = ambientPower;

    //diffuse
    vec4 lightDirection = normalize(lightPos - vertexPosition_worldSpace);
    float diffuseTotal = diffusePower * max(0,dot(lightDirection,normalize(vertexNormal_worldSpace)));


    //specular
    vec4 reflectDirection = reflect(-lightDirection,normalize(vertexNormal_worldSpace));
    vec4 cameraDirection = normalize(vec4(cameraPosition,1) - vertexPosition_worldSpace);
    float specularTotal = specularPower * pow(max(0,dot(reflectDirection,cameraDirection)),specularExponent);


	color.a = 1;
	color.rgb = (ambientTotal + diffuseTotal + specularTotal) * terrainColor;
}
