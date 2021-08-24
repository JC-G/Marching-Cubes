#ifndef GPUMARCHINGCUBESGENERATOR_H
#define GPUMARCHINGCUBESGENERATOR_H

#include <GeometryGenerator.h>


class GPUMarchingCubesGenerator : public GeometryGenerator
{
    public:
        GPUMarchingCubesGenerator();
        virtual ~GPUMarchingCubesGenerator();


        void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize) override;
    protected:

    private:

        static const int edgeTable[256];
        //triTable is the same as in CPU, just flattened
        static const int triTable[4096];
        static void loadShaders();
        static bool shadersLoaded;
};

#endif // GPUMARCHINGCUBESGENERATOR_H
