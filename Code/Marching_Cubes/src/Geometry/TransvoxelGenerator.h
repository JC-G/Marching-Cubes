#ifndef TRANSVOXELGENERATOR_H
#define TRANSVOXELGENERATOR_H

#include "GeometryGenerator.h"
#include "SDF.h"
#include "Shader.h"

#include "TransvoxelTables.h"

class TransvoxelGenerator : public GeometryGenerator
{
    public:
        TransvoxelGenerator(SDF* densityFunction, unsigned int edgeIndex);
        virtual ~TransvoxelGenerator();

        void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize) override;

    protected:

    private:

        SDF* densityFunction;
        unsigned int edgeIndex;
        int getDensityBufferSize(glm::uvec3 chunkSize);

        Shader generateShader;
        Shader countShader;
        Shader polygonizeShader;

        GLuint densityValuesBuffer;

        GLuint marchableCounter; //stage 2 -> 3
        GLuint pointCounter; //stage 2 -> out
        GLuint marchableList; //stage 2 -> 3
        GLuint triangleCounter; //stage 3

        GLuint regularCellClassBuffer;
        GLuint regularCellDataBuffer;
        GLuint regularVertexDataBuffer;
        GLuint regularTotalTableBuffer;
};

#endif // TRANSVOXELGENERATOR_H
