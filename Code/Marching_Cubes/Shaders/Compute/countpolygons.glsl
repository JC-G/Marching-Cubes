//GPU Marching Cubes Stage 2 - Calculate number of "marchable" cubes in the chunk (contain geometry), and cubeIndex for each
#extension GL_ARB_shader_atomic_counter_ops : enable
layout(binding = 1) buffer Grid {
    float distanceValues[];
};

layout(binding = 2) uniform atomic_uint marchableCount;
layout(binding = 3) uniform atomic_uint pointCount;

layout(std430,binding = 4) buffer Marchables {
    uvec4 marchableList[]; //x,y,z,cubeIndex
};

layout(local_size_x=8,local_size_y=8,local_size_z=8) in;


void main() {
	uvec3 gid = gl_GlobalInvocationID;
	if (gid.x >= chunkSize.x || gid.y >= chunkSize.y || gid.z >= chunkSize.z)
	{
		return;
	}

	//Marching cubes algorithm - put into grid as per algorithm, calculate cubeIndex
	float gridCells[8];
	uvec3 gridPos[8];

	gridPos[0] = gid;
	gridPos[1] = gid+uvec3(1,0,0);
	gridPos[2] = gid+uvec3(1,1,0);
	gridPos[3] = gid+uvec3(0,1,0);
	gridPos[4] = gid+uvec3(0,0,1);
	gridPos[5] = gid+uvec3(1,0,1);
	gridPos[6] = gid+uvec3(1,1,1);
	gridPos[7] = gid+uvec3(0,1,1);


	for (int i = 0; i < 8; i++)
	{
		gridCells[i] = distanceValues[getArrID(gridPos[i])];
	}


	int cubeIndex = 0;

	if (gridCells[0] > 0) cubeIndex |= 1;
	if (gridCells[1] > 0) cubeIndex |= 2;
	if (gridCells[2] > 0) cubeIndex |= 4;
	if (gridCells[3] > 0) cubeIndex |= 8;
	if (gridCells[4] > 0) cubeIndex |= 16;
	if (gridCells[5] > 0) cubeIndex |= 32;
	if (gridCells[6] > 0) cubeIndex |= 64;
	if (gridCells[7] > 0) cubeIndex |= 128;


	if (cubeIndex != 0 && cubeIndex != 255)
	{
		atomicCounterAddARB(pointCount,totalTable[cubeIndex]);
		uint bufferIndex = atomicCounterIncrement(marchableCount);
		uvec4 mc = uvec4(gid.x,gid.y,gid.z,cubeIndex);
		marchableList[bufferIndex] = mc;
	}

}
