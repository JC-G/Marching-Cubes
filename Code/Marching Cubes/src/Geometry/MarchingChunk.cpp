#include "MarchingChunk.h"
#include <iostream>
MarchingChunk::MarchingChunk(glm::vec3 chunkLocation, glm::vec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator)
   :myLocation(chunkLocation),mySize(chunkSize),myStride(chunkStride),Generator(Generator)
{
    //ctor
}

MarchingChunk::~MarchingChunk() {
    //dtor
}

void MarchingChunk::draw() {

}


