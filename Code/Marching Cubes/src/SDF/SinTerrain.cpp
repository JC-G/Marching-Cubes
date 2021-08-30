#include "SinTerrain.h"

SinTerrain::SinTerrain()
{
    //ctor
}

SinTerrain::~SinTerrain()
{
    //dtor
}

float SinTerrain::density(glm::vec3 inPos)
{
    return inPos.y - 1.0 * glm::sin(inPos.x+inPos.z);
}

glm::vec3 SinTerrain::normal(glm::vec3 inPos)
{
    //derivative of sin(x+z)-y wrt x,y,z
    return glm::normalize(glm::vec3(
        -1.0 * glm::cos(inPos.x + inPos.z),
        1.0,
        -1.0 * glm::cos(inPos.x + inPos.z)
    ));
}

std::string SinTerrain::getShaderCode()
{
    std::stringstream ss;
    ss <<
        "float density(vec3 inPos) {"
        "   return inPos.y - 1.0 * sin(inPos.x + inPos.z);"
        "}"
        "vec3 normal(vec3 inPos) {"
        "   return normalize(vec3("
        "       -1.0 * cos(inPos.x + inPos.z),"
        "       1.0,"
        "       -1.0 * cos(inPos.x + inPos.z)"
        "   ));"
        "}";
    return ss.str();
}
