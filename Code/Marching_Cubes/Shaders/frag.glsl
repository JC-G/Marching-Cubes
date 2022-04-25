//basic test fragment shader
#version 330

precision highp float;
layout(location = 0) out vec4 color;

in vec4 vertexPosition_worldSpace;
in vec4 vertexNormal_worldSpace;

const vec4 lightPos = vec4(0,100000,0,1);
const vec4 lightColor = vec4(1);
const float ambientPower = .05;
const float diffusePower = .95;
const float specularPower = 1.0;
const float specularExponent = 50.0;
uniform vec3 terrainColor = vec3(0.1,0.5,0.1);

const float nearFog = 4500.;
const float farFog = 5000.;
const vec3 fogColor = vec3(0,148./255,1);

const float textureScale = 8.;

uniform sampler2D rockSampler;
uniform sampler2D grassSampler;

uniform vec3 cameraPosition;

void main() {
    //texture
    vec3 tUVW = mod(vertexPosition_worldSpace.xyz/textureScale,1.);
    //triplanar mapping
    vec3 blending = normalize(abs(vertexNormal_worldSpace.xyz*vec3(4.,1.,4.)));
    blending /= dot(vec3(1.),blending);

    vec3 textureColor = blending.x * texture(rockSampler,tUVW.yz).rgb +
                        blending.y * texture(grassSampler,tUVW.xz).rgb + 
                        blending.z * texture(rockSampler,tUVW.xy).rgb;


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
	vec3 terrainColor = (ambientTotal + diffuseTotal + specularTotal) * textureColor;
    // color.rgb = (vertexNormal_worldSpace.xyz + vec3(1.))/2.;

    //Fog
    float fogFactor = smoothstep(nearFog,farFog,length(cameraPosition-vertexPosition_worldSpace.xyz));
    color.rgb = mix(terrainColor,fogColor,fogFactor);
}
