#ifndef MARCHINGCHUNK_H
#define MARCHINGCHUNK_H

/**
A MarchingChunk is the most basic piece of geometry generated using the marching cubes algorithm

it has:
    a location (the corner with smallest x,y,z coordinates)
    a size (number of cubes in each direction - note the number of points in each direction is one more than this)
    a stride (distance in between points)

it obtains its geometry from a GeometryGenerator, written into vertexBuffer and normalBuffer

*/

#include "GeometryGenerator.h"
#include "Config.h"
#include <glm/gtx/intersect.hpp>

class MarchingChunk
{
    public:
        MarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator, int edgeIndex = 0);
        virtual ~MarchingChunk();
        void draw(GLuint VAO);
        bool hasGeometry();
        void drawBoundary(GLuint VAO);

        void generateBoundary();

        float getIntersectionPoint(glm::vec3 origin, glm::vec3 direction);

    protected:

    private:
        glm::vec3 myLocation;
        glm::vec3 mySize;
        glm::vec3 myStride;
        GeometryGenerator* Generator;
        GLuint myGeometrySize;

        GLuint vertexBuffer;
        GLuint normalBuffer;
        GLuint boundaryBuffer;
};

#endif // MARCHINGCHUNK_H
