//Transvoxel Marching Cubes Stage 3 - Actually generate the geometry

layout(binding = 1) buffer Grid {
    float distanceValues[];
};
uniform uint marchableCount;

layout(std430,binding = 4) buffer Marchables {
    uvec4 marchableList[]; //x,y,z,cubeIndex
};

layout(std430, binding = 5) buffer v {vec4 vertices[];};
layout(std430, binding = 6) buffer n {vec4 normals[];};

layout(binding = 7) uniform atomic_uint triCount;

layout(local_size_x=256,local_size_y=1,local_size_z=1) in;

uniform bool interpolate;
uniform float transitionWidth; //how wide are the transition cells, as a proportion of the normal cells

vec3 VertexInterp(vec3 p1, vec3 p2, float valp1, float valp2) {
    float mu;
    vec3 p;

    if (abs(valp1) < 0.00001)
    {
        return(p1);

    }
    if (abs(valp2) < 0.00001)
    {
        return(p2);

    }
    if (abs(valp1-valp2) < 0.00001)
    {
        return(p1);

    }
    if (!interpolate) {
        mu = 0.5;
    } else {
        mu = -valp1 / (valp2 - valp1);
    }
    p = p1 + mu * (p2 - p1);

    return(p);
}

vec3 alternateVertexPosition(ivec3 vertexPosition) {
    //calculate the position the vertex goes to make space for transition cells
    vec3 newPos = vertexPosition;
    if (vertexPosition.x == 0) {
        newPos.x += transitionWidth;
    }
    if (vertexPosition.x == chunkSize.x) {
        newPos.x -= transitionWidth;
    }
    if (vertexPosition.y == 0) {
        newPos.y += transitionWidth;
    }
    if (vertexPosition.y == chunkSize.y) {
        newPos.y -= transitionWidth;
    }
    if (vertexPosition.z == 0) {
        newPos.z += transitionWidth;
    }
    if (vertexPosition.z == chunkSize.z) {
        newPos.z -= transitionWidth;
    }
    return newPos;
}

int getAdjacentEdges(ivec3 vertexPosition) {
    //return an edgeIndex-like value corresponding to the edges this cell is closest to
    int adjIndex = 0;
    if (vertexPosition.x == 0) {
        adjIndex |= 1;
    }
    if (vertexPosition.x == chunkSize.x) {
        adjIndex |= 2;
    }
    if (vertexPosition.y == 0) {
        adjIndex |= 4;
    }
    if (vertexPosition.y == chunkSize.y) {
        adjIndex |= 8;
    }
    if (vertexPosition.z == 0) {
        adjIndex |= 16;
    }
    if (vertexPosition.z == chunkSize.z) {
        adjIndex |= 32;
    }
    return adjIndex;
}

vec3 actualVertexPosition(ivec3 vertexPosition) {
    if (vertexPosition.x > 0 && vertexPosition.x < chunkSize.x
        && vertexPosition.y > 0 && vertexPosition.y < chunkSize.y
        && vertexPosition.z > 0 && vertexPosition.z < chunkSize.z) {
            return vertexPosition;
    }

    int adjEdges = getAdjacentEdges(vertexPosition);

    //equivalent to bitwise not on a 6-bit integer
    int inverseEdgeIndex = 0x3F ^ (0x3F & edgeIndex);

    //find the edges where we are adjacent to a cell rendering at a lower resolution
    //ie adjacent and not edgeindex
    int lowerResolutionEdges = inverseEdgeIndex & adjEdges;
    int lowerResolutionCount = 0;

    if (lowerResolutionEdges > 0) {
        //use the regular vertex position
        return vertexPosition;
    } else {
        //use the shifted vertex position
        return alternateVertexPosition(vertexPosition);
    }

}

void generateCell() {
    uvec3 gid = marchableList[gl_GlobalInvocationID.x].xyz;

    float gridCells[8];
    ivec3 gridPos[8];

    //assigned as per Figure 3.7 in transvoxel paper - note this is DIFFERENT to the previous marching cubes algorithm
    for (int i = 0; i < 8; i++) {
        gridPos[i] = ivec3(gid) + gridOffset[i];
    }

    //transitionGridPos is the grid positions including the offset from the transition cells
    vec3 transitionGridPos[8];

    //hasShifted is true if the vertex has been moved as a result of being on a transition cell
    bool hasShifted[8];

    for (int i = 0; i < 8; i++) {
        vec3 avp = actualVertexPosition(gridPos[i]);
        transitionGridPos[i] = avp;
        hasShifted[i] = (avp != vec3(gridPos[i]));
    }

    //Actual distance values
    for (int i = 0; i < 8; i++) {
        gridCells[i] = distanceValues[getArrID(gridPos[i],uvec3(0))];
    }

	uint cellIndex = marchableList[gl_GlobalInvocationID.x].w & ((1<<9)-1);
	uint cellClass = regularCellClass[cellIndex];
	uint totalPoints = regularTotalTable[cellClass];

    uint index = 0;
    for (int i = 0; i < totalPoints; i++) {
        if (i % 3 == 0) {
            index = atomicCounterIncrement(triCount);
        }
        uint vertexIndex = index * 3 + (i % 3);

        uint vertexData = regularVertexData[cellIndex * 12 + regularCellData[cellClass * 16 + 1+i]];
        uint v1Index = vertexData & 0x0F;
        uint v2Index = vertexData >> 4;

        vec3 vertPos = VertexInterp(transitionGridPos[v1Index],transitionGridPos[v2Index],gridCells[v1Index],gridCells[v2Index]);// * chunkStride + chunkPosition;

        if (hasShifted[v1Index] || hasShifted[v2Index]) { //if this vertex has moved
            //apply the transformation as in Figure 4.12

            //where the vertex would have been
            vec3 vp2 = VertexInterp(gridPos[v1Index],gridPos[v2Index],gridCells[v1Index], gridCells[v2Index]);
            //normal at this position - in world space
            vec3 n = modified_normal(vp2 * chunkStride + chunkPosition);

            vec3 dv = vertPos - vp2;
            vertPos -= (dot(n,dv)) * n;
        }

        vertPos = vertPos * chunkStride + chunkPosition;

        vertices[vertexIndex] = vec4(vertPos,1);
        normals[vertexIndex] = vec4(modified_normal(vertPos),0);
    }
}

void generateTransitionCell() {
    uvec3 gid = marchableList[gl_GlobalInvocationID.x].xyz;

    ivec3 integerFace[4];
    vec3 transitionGridPos[13];
    float transitionGridCells[13];
    //bits 10-15 describe the orientation of the cell, in the same way as edgeIndex
    int mask = int(marchableList[gl_GlobalInvocationID.x].w) >> 10;
	if (doGenerateTransitionCell(gid,mask)) {
        ivec3 cellEdges = ivec3(0);
        if (mask == 2) {
            cellEdges.x = 1;
        }
        if (mask == 8) {
            cellEdges.y = 1;
        }
        if (mask == 32) {
            cellEdges.z = 1;
        }

        //transitionGridPos is the actual vertex positions - as labelled in Figure 4.16
        //keep these as uvec3 so we can use actualVertexPosition
        ivec3 directionData = getDirectionData(mask);
        ivec3 transformedDirectionData = transformDirectionData(directionData);
        ivec3 integerFace[4];

        int integerFaceCounter = 0;
        for (int zi = 0; zi < 2; zi++) {
            for (int yi = 0; yi < 2; yi++) {
                for (int xi = 0; xi < 2; xi++) {
                    // if component is -1, start at gid+1
                    // if component is +1, start at gid
                    // if component is 0, do not move this coordinate
                    integerFace[integerFaceCounter] = ivec3(gid) + cellEdges + (ivec3(xi,yi,zi) * directionData) + transformedDirectionData;
                    integerFaceCounter++;
                    if (directionData.x == 0) break;
                }
                if (directionData.y == 0) break;
            }
            if (directionData.z == 0) break;
        }

        vec3 transitionGridPos[13];

        int transitionCellCounter = 0;
        for (int zi = 0; zi < 3; zi++) {
            for (int yi = 0; yi < 3; yi++) {
                for (int xi = 0; xi < 3; xi++) {
                    ivec3 actualPosition = getActualPosition(ivec3(xi,yi,zi),directionData);
                    transitionGridPos[transitionCellCounter] = vec3(gid + cellEdges + getFullPart(actualPosition)) + 0.5 * vec3(getHalfPart(actualPosition));
                    transitionCellCounter++;
                    if (directionData.x == 0) break;
                }
                if (directionData.y == 0) break;
            }
            if (directionData.z == 0) break;
        }

        //change the shape of the transition cell at the end of the chunk - Figure 4.13
        transitionGridPos[9 ] = actualVertexPosition(integerFace[0]);
        transitionGridPos[10] = actualVertexPosition(integerFace[1]);
        transitionGridPos[11] = actualVertexPosition(integerFace[2]);
        transitionGridPos[12] = actualVertexPosition(integerFace[3]);

        float transitionGridCells[13];
        //TODO - this is duplicate from transvoxel_count
        int gridCellCounter = 0;
        for (int zi = 0; zi < 3; zi++) {
            for (int yi = 0; yi < 3; yi++) {
                for (int xi = 0; xi < 3; xi++) {
                    ivec3 actualPosition = getActualPosition(ivec3(xi,yi,zi),directionData);
                    transitionGridCells[gridCellCounter] = distanceValues[getArrID(gid + cellEdges + getFullPart(actualPosition), getHalfPart(actualPosition))];
                    gridCellCounter++;
                    if (directionData.x == 0) break;
                }
                if (directionData.y == 0) break;
            }
            if (directionData.z == 0) break;
        }

        transitionGridCells[9 ] = transitionGridCells[0];
        transitionGridCells[10] = transitionGridCells[2];
        transitionGridCells[11] = transitionGridCells[6];
        transitionGridCells[12] = transitionGridCells[8];

        uint paddedTransitionCellIndex = marchableList[gl_GlobalInvocationID.x].w;
        uint transitionCellIndex = paddedTransitionCellIndex & ((1<<9)-1);
        uint cellClass = transitionCellClass[transitionCellIndex];
        uint totalPoints = transitionTotalTable[cellClass & 0x7F];

        uint index = 0;
        for (int i = 0; i < totalPoints; i++) {
            if (i % 3 == 0) {
                index = atomicCounterIncrement(triCount);
            }

            uint vertexIndex;
            if (cellClass >= 0x7f) {
                vertexIndex = index * 3 + 2 - (i % 3);
            } else {
                vertexIndex = index * 3 + (i % 3);
            }

            uint vertexData = transitionVertexData[transitionCellIndex * 12 + transitionCellData[(cellClass & 0x7F) * 37 + 1+i]];
            uint v1Index = vertexData & 0x0F;
            uint v2Index = vertexData >> 4;

            vec3 vertPos = VertexInterp(transitionGridPos[v1Index],transitionGridPos[v2Index],transitionGridCells[v1Index],transitionGridCells[v2Index]);// * chunkStride + chunkPosition;

            //the back face has always been shifted, the front face has never been shifted
            bool hasShifted[13] = {false,false,false,
                                   false,false,false,
                                   false,false,false,
                                   true,true,
                                   true,true};

            vec3 gridPos[13];
            for (int i = 0; i < 9; i++) {
                gridPos[i] = transitionGridPos[i];
            }
            for (int i = 9; i < 13; i++) {
                gridPos[i] = integerFace[i-9];
            }

            if (hasShifted[v1Index] || hasShifted[v2Index]) { //if this vertex has moved
                //apply the transformation as in Figure 4.12
                
                //where the vertex would have been
                vec3 vp2 = VertexInterp(gridPos[v1Index],gridPos[v2Index],transitionGridCells[v1Index], transitionGridCells[v2Index]);
                //normal at this position - in world space
                vec3 n = modified_normal(vp2 * chunkStride + chunkPosition);

                vec3 dv = vertPos - vp2;
                vertPos -= (dot(n,dv)) * n;
            }
            
            vertPos = vertPos * chunkStride + chunkPosition;

            vertices[vertexIndex] = vec4(vertPos,1);
            normals[vertexIndex] = vec4(modified_normal(vertPos),0);
        }

	}
    return;
}

void main() {
    if (gl_GlobalInvocationID.x >= marchableCount) {
        return;
    }

    if (marchableList[gl_GlobalInvocationID.x].w > 255) {
        if (generateTransitionCells) {
            generateTransitionCell();
        }
    } else {
        if (generateRegularCells) {
            generateCell();
        }
    }

}



