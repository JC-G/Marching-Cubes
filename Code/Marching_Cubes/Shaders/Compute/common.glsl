//Common code to be included in all marching cubes compute shaders
//Unneeded methods are optimized out by the compiler

//Chunk Information
uniform uvec3 chunkSize;
uniform vec3 chunkStride;
uniform vec3 chunkPosition;

//MC Algorithm Tables
layout(binding = 8 ) buffer edt {int edgeTable[];};
layout(binding = 9 ) buffer trt {int triTable[];};
layout(binding = 10) buffer tot {int totalTable[];}; //number of triangles within cube with given cubeIndex;

//glsl needs single dimensional arrays - so store it flattened
uint getArrID(uvec3 gid) {
    return gid.x + gid.y * (1+chunkSize.x) + gid.z * (1+chunkSize.x) * (1+chunkSize.y);
}
