#include "NoiseTerrain.h"

NoiseTerrain::NoiseTerrain()
{
    //ctor
}

NoiseTerrain::~NoiseTerrain()
{
    //dtor
}
float NoiseTerrain::distance(glm::vec3 inPos)
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
    "float distance(vec3 inPos) {\n"
    "   return inPos.y - 5.*terrain(inPos.xz / 5);\n"
    "}\n"
    "vec3 normal(vec3 inPos) {\n"
    "    vec2 terrDer = terrainDerivative(inPos.xz/5);"
    "   return normalize(vec3(-terrDer.x,1.0,-terrDer.y));"
    "}\n";


}
