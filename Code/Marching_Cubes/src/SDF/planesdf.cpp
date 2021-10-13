#include "planesdf.h"

PlaneSDF::PlaneSDF()
{
    //ctor
}

PlaneSDF::~PlaneSDF()
{
    //dtor
}
float PlaneSDF::density(glm::vec3 inPos)
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
    "float density (vec3 inPos) { return inPos.x + inPos.y + inPos.z; }"
    "vec3 normal(vec3 inPos) { return normalize(vec3(1,1,1)); }";
}
