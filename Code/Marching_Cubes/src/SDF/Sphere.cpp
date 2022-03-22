#include "Sphere.h"

Sphere::~Sphere()
{
    //dtor
}

Sphere::Sphere(glm::vec3 center, float radius)
    :center(center),radius(radius)
{

}

float Sphere::distance(glm::vec3 inPos)
{
    return glm::length(inPos - center) - radius;
}

glm::vec3 Sphere::normal(glm::vec3 inPos)
{
    return glm::normalize(inPos - center);
}

std::string Sphere::getShaderCode()
{
    std::stringstream ss;
    ss <<
        "float distance(vec3 inPos) {"
        "   return length(inPos - " << glm::to_string(center) << ") - " << radius << ";"
        "}"
        "vec3 normal(vec3 inPos) {"
        "   return normalize(inPos - " << glm::to_string(center) << ");"
        "}";

    return ss.str();
}
