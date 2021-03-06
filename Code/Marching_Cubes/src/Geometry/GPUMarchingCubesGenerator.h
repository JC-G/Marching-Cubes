#ifndef GPUMARCHINGCUBESGENERATOR_H
#define GPUMARCHINGCUBESGENERATOR_H

#include "GeometryGenerator.h"
#include "Shader.h"
#include "SDF.h"

class GPUMarchingCubesGenerator : public GeometryGenerator
{
    public:
        GPUMarchingCubesGenerator(SDF* distanceFunction);
        virtual ~GPUMarchingCubesGenerator();


        void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize, int edgeIndex = 0,const std::vector<BrushParams>& brushes = std::vector<BrushParams>()) override;
    protected:

    private:

        static const int edgeTable[256];
        static const int triTable[4096]; //triTable is the same as in CPU, just flattened
        static const int totalTable[256];

        Shader stage1Shader;
        Shader stage2Shader;
        Shader stage3Shader;

        GLuint distanceValuesBuffer;
        GLuint brushBuffer;

        GLuint marchableCounter; //stage 2 -> 3
        GLuint pointCounter; //stage 2 -> out
        GLuint marchableList; //stage 2 -> 3
        GLuint triangleCounter; //stage 3

        GLuint triTableBuffer;
        GLuint totalTableBuffer;
        GLuint edgeTableBuffer;

        SDF* distanceFunction;


};

#endif // GPUMARCHINGCUBESGENERATOR_H
