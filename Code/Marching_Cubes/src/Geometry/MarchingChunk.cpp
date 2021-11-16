#include "MarchingChunk.h"
#include <iostream>
MarchingChunk::MarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator, int edgeIndex)
   :myLocation(chunkLocation),mySize(chunkSize),myStride(chunkStride),Generator(Generator)
{
    //ctor
    Generator->GenerateGeometry(myLocation,mySize,myStride,&vertexBuffer,&normalBuffer, &myGeometrySize, edgeIndex);
    if (Config::get<bool>("draw_chunk_boundaries")) {
        generateBoundary();
    }
}

MarchingChunk::~MarchingChunk() {
    //dtor
    glDeleteBuffers(1,&vertexBuffer);
    glDeleteBuffers(1,&normalBuffer);

    glDeleteBuffers(1,&boundaryBuffer);
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
    if (Config::wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
    glDrawArrays(GL_TRIANGLES, 0, myGeometrySize);
}

void MarchingChunk::drawBoundary(GLuint VAO) {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,boundaryBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT,GL_FALSE,0,NULL);

    glDrawArrays(GL_LINES,0,24);
}

bool MarchingChunk::hasGeometry() {
    return myGeometrySize > 0;
}

void MarchingChunk::generateBoundary() {
    glm::vec4 vertexData[] = {
        //x oriented
        glm::vec4(myLocation,1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,0,0),1),

        glm::vec4(myLocation + glm::vec3(0,mySize.y * myStride.y,0),1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,mySize.y * myStride.y,0),1),

        glm::vec4(myLocation + glm::vec3(0,mySize.y * myStride.y,mySize.z * myStride.z),1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,mySize.y * myStride.y,mySize.z * myStride.z),1),

        glm::vec4(myLocation + glm::vec3(0,0,mySize.z * myStride.z),1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,0,mySize.z * myStride.z),1),
        
        //y oriented
        glm::vec4(myLocation,1), 
        glm::vec4(myLocation + glm::vec3(0,mySize.y * myStride.y,0),1),

        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,0,0),1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,mySize.y * myStride.y,0),1),

        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,0,mySize.z * myStride.z),1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,mySize.y * myStride.y,mySize.z * myStride.z),1),

        glm::vec4(myLocation + glm::vec3(0,0,mySize.z * myStride.z),1), 
        glm::vec4(myLocation + glm::vec3(0,mySize.y * myStride.y,mySize.z * myStride.z),1),
        
        //z oriented
        glm::vec4(myLocation,1), 
        glm::vec4(myLocation + glm::vec3(0,0,mySize.z * myStride.z),1),

        glm::vec4(myLocation + glm::vec3(0,mySize.y * myStride.y,0),1), 
        glm::vec4(myLocation + glm::vec3(0,mySize.y * myStride.y,mySize.z * myStride.z),1),

        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,mySize.y * myStride.y,0),1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,mySize.y * myStride.y,mySize.z * myStride.z),1),

        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,0,0),1), 
        glm::vec4(myLocation + glm::vec3(mySize.x * myStride.x,0,mySize.z * myStride.z),1),
    };
    glGenBuffers(1,&boundaryBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,boundaryBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData),vertexData,GL_STATIC_DRAW);
}