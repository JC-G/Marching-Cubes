#include "NoiseTerrain.h"

NoiseTerrain::NoiseTerrain()
{
    //ctor
}

NoiseTerrain::~NoiseTerrain()
{
    //dtor
}
float NoiseTerrain::density(glm::vec3 inPos)
{
    //TODO - decide if we want the option of running this on the CPU
    return 0.0f;
}

glm::vec3 NoiseTerrain::normal(glm::vec3 inPos)
{
    return glm::vec3(0,1,0);
}

std::string NoiseTerrain::getShaderCode()
{
    //TODO - find the source of this... probably iq
    return
    "const float HASHSCALE1 = .1031;\n"
    "float hash12(vec2 p)\n"
    "{\n"
    "    vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);\n"
    "    p3 += dot(p3, p3.yzx + 19.19);\n"
    "    return fract((p3.x + p3.y) * p3.z);\n"
    "}\n"

    "vec3 noised2(vec2 x)\n"
    "{\n"
    "    vec2 f = fract(x);\n"
    "    vec2 u = f*f*(3.0-2.0*f);\n"

    "    vec2 p = floor(x);\n"
    "    float a = hash12(p+vec2(0,0));\n"
    "    float b = hash12(p+vec2(1,0));\n"
    "    float c = hash12(p+vec2(0,1));\n"
    "    float d = hash12(p+vec2(1,1));\n"
    "    return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,\n"
    "           6.0*f*(1.0-f)*(vec2(b-a,c-a)+vec2(a-b-c+d)*u.yx));\n"
    "}\n"
    "float terrain(vec2 p) { //in xz, out y\n"
    "   float total = 0.0;\n"
    "   float max = 0.0;\n"
    "   float heightScale = 1.0;\n"
    "   int octaves = 7;\n"
    "   for (int i = 0; i < octaves; i++) {\n"
    "       float n = noised2(p).x;\n"
    "       total += n * heightScale;\n"
    "       max += heightScale;\n"
    "       heightScale *= 0.5;\n"
    "       p = 2.0 * p;\n"
    "   }\n"
    "   return total/max;\n"
    "}\n"
    "float density(vec3 inPos) {\n"
    "   return inPos.y - 5.*terrain(inPos.xz / 5);\n"
    "}\n"
    "vec3 normal(vec3 inPos) {\n"
    "    float max = 0.0;\n"
    "    float heightScale = 1.0;\n"
    "    int octaves = 7;\n"
    "    vec2 terrDer = vec2(0.0);"
    "    vec2 p = inPos.xz / 5;"
    "    for (int i = 0; i < octaves; i++) {"
    "       max += heightScale;"
    "       terrDer += noised2(p).yz;"
    "       p = 2.0 * p;"
    "       heightScale *= 0.5;"
    "   }"
    "   return normalize(vec3(-terrDer.x,1.0,-terrDer.y));"
    "}\n";


}
