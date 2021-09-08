#include "TestSDF.h"

TestSDF::TestSDF()
{

}

TestSDF::~TestSDF()
{

}

float TestSDF::density(glm::vec3 inPos)
{
    return 0.0;
}

glm::vec3 TestSDF::normal(glm::vec3 inPos)
{
    return glm::vec3(0,1,0);
}

std::string TestSDF::getShaderCode()
{
    return
    "float density(vec3 inPos) {"
    "   return inPos.y-inPos.x-inPos.z;"
    "}"
    "vec3 normal(vec3 inPos) {"
    "   return normalize(vec3(-1,1,-1));"
    "}";
}
