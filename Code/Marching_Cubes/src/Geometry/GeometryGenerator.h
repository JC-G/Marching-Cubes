#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

/**
GeometryGenerator is an abstract class handling generation of geometry for MarchingChunk objects
it fills the given vertex and normal buffers
*/

#include "GL_headers.h"

class GeometryGenerator
{
    public:
        GeometryGenerator();
        virtual ~GeometryGenerator();
        // return the VBO - assume we have a VAO for now
        virtual void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize) = 0;
    protected:

    private:
};

#endif // GEOMETRYGENERATOR_H
