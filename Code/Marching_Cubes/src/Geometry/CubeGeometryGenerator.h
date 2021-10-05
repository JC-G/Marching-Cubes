#ifndef CUBEGEOMETRYGENERATOR_H
#define CUBEGEOMETRYGENERATOR_H

#include "Geometry/GeometryGenerator.h"
/*
Test class - generates only a unit cube at the origin
*/

class CubeGeometryGenerator : public GeometryGenerator
{
    public:
        CubeGeometryGenerator();
        virtual ~CubeGeometryGenerator();

        void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize, int edgeIndex) override;
    protected:

    private:
};

#endif // CUBEGEOMETRYGENERATOR_H
