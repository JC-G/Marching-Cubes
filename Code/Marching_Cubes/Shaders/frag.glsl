//basic test fragment shader
#version 400

precision highp float;
layout(location = 0) out vec4 color;

in vec4 vertexPosition_worldSpace;
in vec4 vertexNormal_worldSpace;


//Lighting Parameters
const vec4 sunPos = vec4(50000,100000,50000,1);
const vec4 sunColor = vec4(1);
const vec4 playerLightOffset = vec4(0,1,0,0);
const vec4 playerLightColor = vec4(1);
const float ambientPower = .05;
const float diffusePower = .95;
const float specularPower = .50;
const float specularExponent = 10.0;


//Shading Parameters
const vec3 grassColor = vec3(38.,127.,0.)/255.;
const vec3 grassColor2 = vec3(20.,60.,10.)/255.;
// const vec3 grassColor2 = vec3(255.,0.,10.)/255.;


const vec3 rockColor = vec3(192.)/255.;
const vec3 rockColor2 = vec3(50.)/255.;

const float grassPow = 2.0;
const float nearFog = 4500.;
const float farFog = 5000.;
const vec3 fogColor = vec3(0,148./255,1);

uniform vec3 cameraPosition;



//3D noise
const float HASHSCALE1 = .1031;
float hash13(vec3 p3)
{
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float noise3( in vec3 x )
{
    vec3 p = floor(x);
    vec3 w = fract(x);
    
    vec3 u = w*w*w*(w*(w*6.0-15.0)+10.0);

    float a = hash13( p+vec3(0,0,0) );
    float b = hash13( p+vec3(1,0,0) );
    float c = hash13( p+vec3(0,1,0) );
    float d = hash13( p+vec3(1,1,0) );
    float e = hash13( p+vec3(0,0,1) );
    float f = hash13( p+vec3(1,0,1) );
    float g = hash13( p+vec3(0,1,1) );
    float h = hash13( p+vec3(1,1,1) );

    float k0 =   a;
    float k1 =   b - a;
    float k2 =   c - a;
    float k3 =   e - a;
    float k4 =   a - b - c + d;
    float k5 =   a - c - e + g;
    float k6 =   a - b - e + f;
    float k7 = - a + b + c - d + e - f - g + h;

    return (k0 + k1*u.x + k2*u.y + k3*u.z + k4*u.x*u.y + k5*u.y*u.z + k6*u.z*u.x + k7*u.x*u.y*u.z);
}

float perlin3(vec3 x,int octaves) //multiple octaves of noise3
{
	float squish = 1.0;
	float scale = 1.0;
	float T = 0.;
	float res = 0.;
	for(int i=0;i<octaves;i++)
	{
		res+=scale*noise3(x*squish);
		T+=scale;
		scale*=.9;
		squish*=2.;
	}
	return res/T;
}

void main() {
    //Amount of grass is based on angle with up
    float d2c = length(vertexPosition_worldSpace.xyz-cameraPosition);
    float octaveFactor = 10.-clamp(d2c/200.,0.,8.);
    int octaves = int(floor(octaveFactor));
    octaves = 10;

    float grassAmount = smoothstep(0.75,0.9,vertexNormal_worldSpace.y);
    float grassBlend = perlin3(vertexPosition_worldSpace.xyz/10.,octaves);
    float rockBlend = perlin3(vertexPosition_worldSpace.xyz/vec3(50.,8.,50.),octaves);

    vec3 thisGrassColor = mix(grassColor,grassColor2,grassBlend);
    vec3 thisRockColor = mix(rockColor,rockColor2,rockBlend);

    vec3 textureColor = grassAmount * thisGrassColor + (1.0-grassAmount) * thisRockColor;

    //ambient
    float ambientTotal = ambientPower;

    //diffuse - from player and from sun
    vec4 sunDirection = normalize(sunPos - vertexPosition_worldSpace);
    vec4 playerLightPosition = playerLightOffset + vec4(cameraPosition,0.);
    vec4 playerLightDirection = normalize(playerLightPosition - vertexPosition_worldSpace);
    float diffuseTotal = diffusePower * (
        0.5 * max(0,dot(sunDirection,normalize(vertexNormal_worldSpace))) +
        0.5 * max(0,dot(playerLightDirection,normalize(vertexNormal_worldSpace)))
    );


    //specular - only from sun...
    vec4 reflectDirection = reflect(-sunDirection,normalize(vertexNormal_worldSpace));
    vec4 cameraDirection = normalize(vec4(cameraPosition,1) - vertexPosition_worldSpace);
    float specularTotal = specularPower * pow(max(0,dot(reflectDirection,cameraDirection)),specularExponent);


	color.a = 1;
	vec3 terrainColor = (ambientTotal + diffuseTotal + specularTotal) * textureColor;
    // color.rgb = (vertexNormal_worldSpace.xyz + vec3(1.))/2.;

    //Fog
    float fogFactor = smoothstep(nearFog,farFog,length(cameraPosition-vertexPosition_worldSpace.xyz));
    color.rgb = mix(terrainColor,fogColor,fogFactor);
}
