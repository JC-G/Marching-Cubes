
//Chunk Information
uniform uvec3 chunkSize;
uniform vec3 chunkStride;
uniform vec3 chunkPosition;
uniform int edgeIndex; //where are extra values needed

//Debug generation options
uniform bool generateTransitionCells;
uniform bool generateRegularCells;

//Transvoxel Algorithm Tables
layout(binding = 8 ) buffer rcc {int regularCellClass[];};
layout(binding = 9 ) buffer rcd {int regularCellData[];};
layout(binding = 10) buffer rvd {int regularVertexData[];}; //number of triangles within cube with given cubeIndex;
layout(binding = 11) buffer rtt {int regularTotalTable[];};

layout(binding = 12) buffer tcc {int transitionCellClass[];};
layout(binding = 13) buffer tcd {int transitionCellData[];};
layout(binding = 14) buffer tvd {int transitionVertexData[];}; //number of triangles within cube with given cubeIndex;
layout(binding = 15) buffer ttt {int transitionTotalTable[];};

const ivec3 gridOffset[8] = ivec3[](
    ivec3(0,0,0),
    ivec3(1,0,0),
    ivec3(0,1,0),
    ivec3(1,1,0),
    ivec3(0,0,1),
    ivec3(1,0,1),
    ivec3(0,1,1),
    ivec3(1,1,1)
);

//should be as close to a bijection as possible - minimise the values that need to be generated twice
//gid is between 0 and chunkSize inclusive
//halfXYZ is 0 or 1 for half values

//Changes here may affect TransvoxelGenerator::getDistanceBufferSize
uint getArrID(uvec3 gid, uvec3 halfXYZ) {
    //Store the main volume like this
    if (gid.x > 0 && gid.x < chunkSize.x &&
        gid.y > 0 && gid.y < chunkSize.y &&
        gid.z > 0 && gid.z < chunkSize.z) {
        return (gid.x - 1) +
               (gid.y - 1) * (chunkSize.x - 1) +
               (gid.z - 1) * (chunkSize.x - 1) * (chunkSize.y - 1); 
    }

    uint offset = (chunkSize.x - 1) * (chunkSize.y - 1) * (chunkSize.z - 1);

    uvec3 temp = 2 * gid + halfXYZ;
    uvec3 ts = 2 * chunkSize + uvec3(1);
    //Store the faces as -x, +x, -y, +y, -z, +z in that order, edges and corners are stored in the first place in this ordering (some unpopulated values)

    //point on the -x face
    if (gid.x == 0 && halfXYZ.x == 0) {
        return offset +
            temp.y + ts.y * temp.z;
    }
    offset += ts.y * ts.z;
    //point on the +x face
    if (gid.x == chunkSize.x && halfXYZ.x == 0) {
        return offset +
            temp.y + ts.y * temp.z;
    }
    offset += ts.y * ts.z;

    //point on the -y face
    if (gid.y == 0 && halfXYZ.y == 0) {
        return offset +
            temp.x + ts.x * temp.z;
    }
    offset += ts.x * ts.z;
    //point on the +y face
    if (gid.y == chunkSize.y && halfXYZ.y == 0) {
        return offset +
            temp.x + ts.x * temp.z;
    }
    offset += ts.x * ts.z;

    //point on the -z face
    if (gid.z == 0 && halfXYZ.z == 0) {
        return offset +
            temp.x + ts.x * temp.y;
    }
    offset += ts.x * ts.y;
    //point on the +z face
    if (gid.z == chunkSize.z && halfXYZ.z == 0) {
        return offset +
            temp.x + ts.x * temp.y;
    }
    offset += ts.x * ts.y;

}

//Utility functions to assist in iterating over vertices of cells

//returns the direction to traverse to get the position of vertices of transition cells
//for example in the -x direction, traverse towards +y,+z
//This ensures triangles in transition cells are oriented correctly
ivec3 getDirectionData(int edgeDirection) {
    if (edgeDirection == 1) { //-x
        return ivec3(0,1,1);
    }
    if (edgeDirection == 2) { //+x
        return ivec3(0,1,-1);
    }
    if (edgeDirection == 4) {
        return ivec3(-1,0,1);
    }
    if (edgeDirection == 8) {
        return ivec3(1,0,1);
    }
    if (edgeDirection == 16) {
        return ivec3(1,1,0);
    }
    if (edgeDirection == 32) {
        return ivec3(-1,1,0);
    }
    return ivec3(0);
}

//map -1 -> 1, 1 -> 0, 0 -> 0
ivec3 transformDirectionData(ivec3 directionData) {
    return (ivec3(1) - directionData)/2;
}


//generate the sequences (0,0,1), (0,1,0) in parallel, coordinate-wise
ivec3 getFullPart(ivec3 inXYZ) { //(0,0,1)
    return (inXYZ+ivec3(1)) / 3; //integer division round-down from (1,2,3) to (0,0,1)
}

ivec3 getHalfPart(ivec3 inXYZ) { //(0,1,0)
    return inXYZ % 2;
}

//Map (0,1,2) to (0,1,2) if directionData = 1, map to (2,1,0) if directionData = -1, map everything to 0 otherwise
ivec3 getActualPosition(ivec3 inXYZ, ivec3 directionData) {
    return (inXYZ + 2 * transformDirectionData(directionData) * (ivec3(1)-inXYZ)) * abs(directionData);
}

//Conditions for generating transition cells
bool doGenerateTransitionCell(uvec3 gid, int mask) {
    if ((edgeIndex & mask) == 0) {
        return false;
    }
    if (mask == 1) { //-x
        return gid.x == 0;
    }
    if (mask == 2) { //+x
        return gid.x == chunkSize.x-1;
    }
    if (mask == 4) { //-y
        return gid.y == 0;
    }
    if (mask == 8) { //+y
        return gid.y == chunkSize.y-1;
    }
    if (mask == 16) { //-z
        return gid.z == 0;
    }
    if (mask == 32) { //+z
        return gid.z == chunkSize.z-1;
    }
}

bool inBox(vec4 bottom, vec4 top, vec3 inPos) {
    return all(lessThanEqual(bottom.xyz,inPos + 2.0 * chunkStride)) && all(lessThanEqual(inPos - 2.0 * chunkStride,top.xyz));
}
