#ifndef TRANSVOXELGENERATOR_H
#define TRANSVOXELGENERATOR_H

#include "GeometryGenerator.h"
#include "SDF.h"
#include "Shader.h"

#include "Brush.h"
#include "EllipsoidBrush.h"
#include "TestBrushes.h"
#include "Editing.h"

#include "TransvoxelTables.h"
#include <bitset>
#include <vector>

class TransvoxelGenerator : public GeometryGenerator
{
    public:
        TransvoxelGenerator(SDF* distanceFunction);
        virtual ~TransvoxelGenerator();

        void GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize, int edgeIndex = 0,const std::vector<BrushParams>& brushes = std::vector<BrushParams>()) override;

    protected:

    private:

        SDF* distanceFunction;
        int getDistanceBufferSize(glm::uvec3 chunkSize, int edgeIndex);

        Shader generateShader;
        Shader countShader;
        Shader polygonizeShader;

        GLuint distanceValuesBuffer;
        GLuint brushBuffer;

        GLuint marchableCounter; //stage 2 -> 3
        GLuint pointCounter; //stage 2 -> out
        GLuint marchableList; //stage 2 -> 3
        GLuint triangleCounter; //stage 3

        GLuint regularCellClassBuffer;
        GLuint regularCellDataBuffer;
        GLuint regularVertexDataBuffer;
        GLuint regularTotalTableBuffer;

        GLuint transitionCellClassBuffer;
        GLuint transitionCellDataBuffer;
        GLuint transitionVertexDataBuffer;
        GLuint transitionTotalTableBuffer;
};

#endif // TRANSVOXELGENERATOR_H
