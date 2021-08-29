#include "MarchingChunk.h"
#include <iostream>
MarchingChunk::MarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator)
   :myLocation(chunkLocation),mySize(chunkSize),myStride(chunkStride),Generator(Generator)
{
    //ctor
    Generator->GenerateGeometry(myLocation,mySize,myStride,&vertexBuffer,&normalBuffer, &myGeometrySize);
}

MarchingChunk::~MarchingChunk() {
    //dtor
}

void MarchingChunk::draw(GLuint VAO) {

    glBindVertexArray(VAO);

    //bind buffer and set VAO to use this at location 0
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);


    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);


    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, myGeometrySize);
}


