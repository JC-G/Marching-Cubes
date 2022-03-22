#include "TestSDF.h"

TestSDF::TestSDF(float scale, float iso)
:scale(scale),iso(iso)
{

}

TestSDF::~TestSDF()
{

}

float TestSDF::distance(glm::vec3 inPos)
{
    return scale * (iso - perlin3((1.0f/scale)* inPos));
}

glm::vec3 TestSDF::normal(glm::vec3 inPos)
{
    float eps = 0.001;
    glm::vec3 dx = inPos + glm::vec3(eps,0,0);
    glm::vec3 dy = inPos + glm::vec3(0,eps,0);
    glm::vec3 dz = inPos + glm::vec3(0,0,eps);

    float f  = distance(inPos);
    float fx = distance(dx);
    float fy = distance(dy);
    float fz = distance(dz);
    return glm::normalize(glm::vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));
}

std::string TestSDF::getShaderCode()
{
    std::stringstream ss;
    ss << "float distance(vec3 inPos) {"
    "   return "<< scale << "* (" << iso << " - perlin3(inPos/" << scale << "));"
    "}"
    "vec3 normal(vec3 inPos) {"
        "float eps = 0.001;"
        "vec3 dx = inPos + vec3(eps,0,0);"
        "vec3 dy = inPos + vec3(0,eps,0);"
        "vec3 dz = inPos + vec3(0,0,eps);"

        "float f  = distance(inPos);"
        "float fx = distance(dx);"
        "float fy = distance(dy);"
        "float fz = distance(dz);"
        "return normalize(vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));"
    "}";
    return ss.str();
}


float TestSDF::hash13(glm::vec3 p3)
{
	p3  = glm::fract(p3 * HASHSCALE1);
    p3 += glm::dot(p3, glm::vec3(p3.y,p3.z,p3.x) + glm::vec3(19.19));
    return glm::fract((p3.x + p3.y) * p3.z);
}

float TestSDF::noise3(glm::vec3 x )
{
    glm::vec3 p = glm::floor(x);
    glm::vec3 w = glm::fract(x);
    
    glm::vec3 u = w*w*w*(w*(w*6.0f-15.0f)+10.0f);

    float a = hash13( p+glm::vec3(0,0,0) );
    float b = hash13( p+glm::vec3(1,0,0) );
    float c = hash13( p+glm::vec3(0,1,0) );
    float d = hash13( p+glm::vec3(1,1,0) );
    float e = hash13( p+glm::vec3(0,0,1) );
    float f = hash13( p+glm::vec3(1,0,1) );
    float g = hash13( p+glm::vec3(0,1,1) );
    float h = hash13( p+glm::vec3(1,1,1) );

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

float TestSDF::perlin3(glm::vec3 x) //multiple octaves of noise3
{
	int octaves = 6;
	float squish = 1.0;
	float scale = 1.0;
	float T = 0.;
	float res = 0.;
	for(int i=0;i<octaves;i++)
	{
		res+=scale*noise3(x*squish);
		T+=scale;
		scale*=.5;
		squish*=2.;
	}
	return res/T;
}
