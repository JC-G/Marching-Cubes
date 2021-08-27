#include "Sphere.h"
#include <sstream>
Sphere::~Sphere()
{
    //dtor
}

Sphere::Sphere(glm::vec3 center, float radius)
    :center(center),radius(radius)
{

}

float Sphere::density(glm::vec3 inPos)
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
        "float density(vec3 inPos) {"
        "   return length(inPos - " << glm::to_string(center) << ") - " << radius << ";"
        "}"
        "vec3 normal(vec3 inPos) {"
        "   return normalize(inPos - " << glm::to_string(center) << ");"
        "}";

    return ss.str();
}
