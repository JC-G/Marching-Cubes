#ifndef CPUMARCHINGCUBESGENERATOR_H
#define CPUMARCHINGCUBESGENERATOR_H

#include "GeometryGenerator.h"
#include "SDF.h"

class CPUMarchingCubesGenerator : public GeometryGenerator
{
    public:
        CPUMarchingCubesGenerator(SDF* distanceFunction);
        virtual ~CPUMarchingCubesGenerator();

        void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize, int edgeIndex,const std::vector<BrushParams>& brushes = std::vector<BrushParams>()) override;
    protected:

    private:
        static const int edgeTable[256];

        static const int triTable[256][16];

        glm::vec3 VertexInterp(glm::vec3 p1, glm::vec3 p2, float valp1, float valp2);

        SDF* distanceFunction;

};

#endif // CPUMARCHINGCUBESGENERATOR_H
