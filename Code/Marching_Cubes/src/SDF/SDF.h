#ifndef SDF_H
#define SDF_H

/**
Base class for signed distance functions
Should implement vertex and normal functions for the CPU,
and a function returning a string defining the vertex and normal functions in GLSL
The normal function must be at least defined everywhere where a marching cubes vertex might be generated, and should closely approximate the normal of the surface when a point is close to the surface

By convention: negative values are INSIDE the surface, positive values are OUTSIDE
*/

#include "GL_headers.h"

#include <sstream>

class SDF
{
    public:
        SDF();
        virtual ~SDF();

        virtual float density(glm::vec3 inPos) = 0;
        virtual glm::vec3 normal(glm::vec3 inPos) = 0;

        virtual std::string getShaderCode() = 0;

    protected:

    private:
};

#endif // SDF_H
