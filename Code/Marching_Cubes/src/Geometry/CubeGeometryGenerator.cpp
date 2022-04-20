#include "CubeGeometryGenerator.h"

/*
Test class - generates only a unit cube at the origin
*/

CubeGeometryGenerator::CubeGeometryGenerator()
{
    //ctor
}

CubeGeometryGenerator::~CubeGeometryGenerator()
{
    //dtor
}
void CubeGeometryGenerator::GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize, int edgeIndex = 0,const std::vector<BrushParams>& brushes)
{
    GLfloat vertexData[] = {

        //  X     Y     Z   W

         -1.0f,-1.0f,-1.0f,1.0f,// triangle 1 : begin
        -1.0f,-1.0f, 1.0f,1.0f,
        -1.0f, 1.0f, 1.0f,1.0f,// triangle 1 : end
        1.0f, 1.0f,-1.0f,1.0f,// triangle 2 : begin
        -1.0f,-1.0f,-1.0f,1.0f,
        -1.0f, 1.0f,-1.0f,1.0f,// triangle 2 : end
        1.0f,-1.0f, 1.0f,1.0f,
        -1.0f,-1.0f,-1.0f,1.0f,
        1.0f,-1.0f,-1.0f,1.0f,
        1.0f, 1.0f,-1.0f,1.0f,
        1.0f,-1.0f,-1.0f,1.0f,
        -1.0f,-1.0f,-1.0f,1.0f,
        -1.0f,-1.0f,-1.0f,1.0f,
        -1.0f, 1.0f, 1.0f,1.0f,
        -1.0f, 1.0f,-1.0f,1.0f,
        1.0f,-1.0f, 1.0f,1.0f,
        -1.0f,-1.0f, 1.0f,1.0f,
        -1.0f,-1.0f,-1.0f,1.0f,
        -1.0f, 1.0f, 1.0f,1.0f,
        -1.0f,-1.0f, 1.0f,1.0f,
        1.0f,-1.0f, 1.0f,1.0f,
        1.0f, 1.0f, 1.0f,1.0f,
        1.0f,-1.0f,-1.0f,1.0f,
        1.0f, 1.0f,-1.0f,1.0f,
        1.0f,-1.0f,-1.0f,1.0f,
        1.0f, 1.0f, 1.0f,1.0f,
        1.0f,-1.0f, 1.0f,1.0f,
        1.0f, 1.0f, 1.0f,1.0f,
        1.0f, 1.0f,-1.0f,1.0f,
        -1.0f, 1.0f,-1.0f,1.0f,
        1.0f, 1.0f, 1.0f,1.0f,
        -1.0f, 1.0f,-1.0f,1.0f,
        -1.0f, 1.0f, 1.0f,1.0f,
        1.0f, 1.0f, 1.0f,1.0f,
        -1.0f, 1.0f, 1.0f,1.0f,
        1.0f,-1.0f, 1.0f,1.0f,
    };

    //Generate a VBO to put generated geometry in
    glGenBuffers(1, vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);

    //put the geometry in
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    //fake normal buffer so the program doesnt crash...
    glGenBuffers(1,normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,*normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);


    //size of geometry
    *geometrySize = 12*4;
}
