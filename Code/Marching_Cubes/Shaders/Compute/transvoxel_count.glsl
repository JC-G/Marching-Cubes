//Transvoxel stage 2 - Calculate number of "marchable" cells, cell classes

#extension GL_ARB_shader_atomic_counter_ops : enable

layout(binding = 1) buffer Grid {
    float distanceValues[];
};

//number of cells to polygonize
layout(binding = 2) uniform atomic_uint marchableCount;

//number of points in the resulting mesh
layout(binding = 3) uniform atomic_uint pointCount;

layout(std430,binding = 4) buffer Marchables {
    uvec4 marchableList[]; //x,y,z, cell index
    //cell index is the lowest 9 bits, (from 0 to 511)
    //cell type (transition or regular) is 10th bit (512, 1<<9)
    //transition cell orientation is the 11th to 16th bit (1024 - 32768, 1<<10 - 15)
};

layout(local_size_x=8,local_size_y=8,local_size_z=8) in;

void main () {
    uvec3 gid = gl_GlobalInvocationID;
    if (gid.x >= chunkSize.x || gid.y >= chunkSize.y || gid.z >= chunkSize.z)
	{
		return;
	}
	if(generateRegularCells) {
		//regular (non-transition) cell

		float gridCells[8];
		uvec3 gridPos[8];

		//assigned as per Figure 3.7 in transvoxel paper - note this is DIFFERENT to the previous marching cubes algorithm
		for (int i = 0; i < 8; i++) {
			gridPos[i] = ivec3(gid) + gridOffset[i];
		}

		for (int i = 0; i < 8; i++)
		{
			gridCells[i] = distanceValues[getArrID(gridPos[i],uvec3(0))];
		}

		int cellIndex = 0;

		int cellMask = 1;
		for (int i = 0; i < 8; i++) {
			if (gridCells[i] < 0) cellIndex |= cellMask;
			cellMask = cellMask << 1;
		}

		if (cellIndex != 0 && cellIndex != 255)
		{
			atomicCounterAddARB(pointCount,regularTotalTable[regularCellClass[cellIndex]]);
			uint bufferIndex = atomicCounterIncrement(marchableCount);
			uvec4 mc = uvec4(gid.x,gid.y,gid.z,cellIndex);
			marchableList[bufferIndex] = mc;
		}
	}

	if (generateTransitionCells) {

		for (int mask = 1; mask <= 32; mask *= 2) {
			float transitionGridCells[9];
			ivec3 transitionGridPos[9];
			int transitionCellIndex = 0;
			if (doGenerateTransitionCell(gid,mask)) {

				//assigned as per Figure 4.16 in transvoxel paper

				ivec3 directionData = getDirectionData(mask);
				ivec3 transformedDirectionData = transformDirectionData(directionData);

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

				//Note the obscure order of this - see Figure 4.17 in transvoxel paper
				//order is 0,1,2,5,8,7,6,3,4
				int transitionFaceOrder[9] = int[](0,1,2,5,8,7,6,3,4);
				int transitionCellMask = 1;
				for (int i = 0; i < 9; i++) {
					if (transitionGridCells[transitionFaceOrder[i]] < 0) transitionCellIndex |= transitionCellMask;
					transitionCellMask = transitionCellMask << 1;
				}

				//cell is a transition cell:
				int paddedTransitionCellIndex = transitionCellIndex;
				paddedTransitionCellIndex |= (1<<9);

				//store cell orientation:
				paddedTransitionCellIndex |= (mask<<10);

				//do not march if all inside or all outside
				if (transitionCellIndex != 0 && transitionCellIndex != 511) {
					//number of points in the mesh
					//and with 0x7f for lookup table
					atomicCounterAddARB(pointCount,transitionTotalTable[0x7F & transitionCellClass[transitionCellIndex]]);

					uint bufferIndex = atomicCounterIncrement(marchableCount);
					uvec4 mc = uvec4(gid.x,gid.y,gid.z,paddedTransitionCellIndex);
					marchableList[bufferIndex] = mc;
				}
			}
		}
	}
}
