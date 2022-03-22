#include "planesdf.h"

PlaneSDF::PlaneSDF()
{
    //ctor
}

PlaneSDF::~PlaneSDF()
{
    //dtor
}
float PlaneSDF::distance(glm::vec3 inPos)
{
    return 0.0f;
}

glm::vec3 PlaneSDF::normal(glm::vec3 inPos)
{
    return glm::vec3(0,1,0);
}

std::string PlaneSDF::getShaderCode()
{
    return
    "float distance (vec3 inPos) { return inPos.y-0.5; }"
    "vec3 normal(vec3 inPos) { return normalize(vec3(0,1,0)); }";
}
