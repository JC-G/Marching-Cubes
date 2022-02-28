#include "MarchingChunk.h"
#include <iostream>

std::vector<MarchingChunk*> MarchingChunk::loadedChunks;

std::shared_ptr<MarchingChunk> MarchingChunk::createMarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator, int edgeIndex, int detailLevel) {
    std::shared_ptr<MarchingChunk> chunk = std::shared_ptr<MarchingChunk>(new MarchingChunk(chunkLocation,chunkSize,chunkStride,Generator, edgeIndex,detailLevel));
    chunk->attachMesh();
    return chunk;
}

void MarchingChunk::attachMesh() {
    if (hasGeometry() && Config::get<bool>("enable_physics") && myDetailLevel >= Config::get<int>("physics_threshold")) {

        myMesh = new ChunkMesh(getptr());
        hasPhysicsMesh = true;
    } else {
        hasPhysicsMesh = false;
    }
}

MarchingChunk::MarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator, int edgeIndex, int detailLevel)
   :myLocation(chunkLocation),mySize(chunkSize),myStride(chunkStride),Generator(Generator), myDetailLevel(detailLevel)
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
    if (Config::get<bool>("draw_chunk_boundaries")) {
        glDeleteBuffers(1,&boundaryBuffer);

    }
    if (hasPhysicsMesh) {
        myMesh->tryDelete();
        // delete myMesh;
    }
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

float MarchingChunk::getIntersectionPoint(glm::vec3 origin, glm::vec3 direction){
    //TODO - do this with a shader rather than mapping?
    float tMin = std::numeric_limits<float>::max();
    if (!hasGeometry()) {
        return tMin;
    }
    mapGeometry();

    for (int i = 0; i < myGeometrySize; i+=3) {        //for each triangle
        //test intersection
        glm::vec2 bary;
        float distance;
        if(glm::intersectRayTriangle(origin, direction, glm::vec3(mappedTriangles[i]), glm::vec3(mappedTriangles[i+1]), glm::vec3(mappedTriangles[i+2]),bary,distance)) {
            float t = distance;
            if (t >= 0 && t < tMin) {
                tMin = t;

            }
        }

    }    
    return tMin;

}

void MarchingChunk::mapGeometry() {
    if (!isMapped) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
        mappedTriangles.resize(myGeometrySize);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER,0,myGeometrySize * sizeof(glm::vec4),mappedTriangles.data());
        //mappedTriangles = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,myGeometrySize,GL_MAP_READ_BIT);
        isMapped = true;
    }
}

int MarchingChunk::getGeometrySize() {
    return myGeometrySize;
}

std::vector<glm::vec4>* MarchingChunk::getGeometryPointer() {
    if (!isMapped) {
        mapGeometry();
    }
    return &mappedTriangles;
}

