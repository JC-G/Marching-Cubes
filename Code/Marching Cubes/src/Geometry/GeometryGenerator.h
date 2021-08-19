#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

/**
GeometryGenerator is an abstract class handling generation of geometry for MarchingChunk objects
it fills the given vertex and normal buffers
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class GeometryGenerator
{
    public:
        GeometryGenerator();
        virtual ~GeometryGenerator();
        virtual GLuint GenerateGeometry(glm::vec3 chunkLocation, glm::vec3 chunkSize, glm::vec3 chunkStride) = 0;
    protected:

    private:
};

#endif // GEOMETRYGENERATOR_H
