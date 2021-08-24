#ifndef CPUMARCHINGCUBESGENERATOR_H
#define CPUMARCHINGCUBESGENERATOR_H

#include <GeometryGenerator.h>


class CPUMarchingCubesGenerator : public GeometryGenerator
{
    public:
        CPUMarchingCubesGenerator();
        virtual ~CPUMarchingCubesGenerator();

        void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize) override;
    protected:

    private:
        static const int edgeTable[256];

        static const int triTable[256][16];

        float densityFunction(glm::vec3 pos);

        glm::vec3 VertexInterp(glm::vec3 p1, glm::vec3 p2, float valp1, float valp2);

};

#endif // CPUMARCHINGCUBESGENERATOR_H
