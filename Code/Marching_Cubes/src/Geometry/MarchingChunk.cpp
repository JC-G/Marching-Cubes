#include "MarchingChunk.h"
#include <iostream>
MarchingChunk::MarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator, int edgeIndex)
   :myLocation(chunkLocation),mySize(chunkSize),myStride(chunkStride),Generator(Generator)
{
    //ctor
    if (edgeIndex != 0) {
        std::cout << "Edge Index " << edgeIndex << std::endl;
    }
    Generator->GenerateGeometry(myLocation,mySize,myStride,&vertexBuffer,&normalBuffer, &myGeometrySize, edgeIndex);
}

MarchingChunk::~MarchingChunk() {
    //dtor
    glDeleteBuffers(1,&vertexBuffer);
    glDeleteBuffers(1,&normalBuffer);
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
    if (Config::get<bool>("draw_lines")) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    glDrawArrays(GL_TRIANGLES, 0, myGeometrySize);
}


