//GPU Marching Cubes Stage 3 - Actually generate the geometry

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
    mu = -valp1 / (valp2 - valp1);
    p = p1 + mu * (p2 - p1);

    return(p);
}


void main() {
	if (gl_GlobalInvocationID.x < marchableCount) {
        uvec3 gid = marchableList[gl_GlobalInvocationID.x].xyz;
		vec3 vertlist[12];

		//initialize to avoid warning
		for (int filler = 0; filler < 12;filler++)
		{
			vertlist[filler]=vec3(0.);
		}

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


        for (int i = 0; i < 8; i++) {
            gridCells[i] = distanceValues[getArrID(gridPos[i])];
        }

        int E = int(edgeTable[marchableList[gl_GlobalInvocationID.x].w]);
        if (E != 0 && E != 255) {
            uint cubeIndex = marchableList[gl_GlobalInvocationID.x].w;
            /* Find the vertices where the surface intersects the cube */
            if ((E & 1) != 0) {
                vertlist[0] = VertexInterp(gridPos[0],gridPos[1],gridCells[0],gridCells[1]);
            }
            if ((E & 2) != 0) {
                vertlist[1] = VertexInterp(gridPos[1],gridPos[2],gridCells[1],gridCells[2]);
            }
            if ((E & 4) != 0) {
                vertlist[2] = VertexInterp(gridPos[2],gridPos[3],gridCells[2],gridCells[3]);
            }
            if ((E & 8) != 0) {
                vertlist[3] = VertexInterp(gridPos[3],gridPos[0],gridCells[3],gridCells[0]);
            }
            if ((E & 16) != 0) {
                vertlist[4] = VertexInterp(gridPos[4],gridPos[5],gridCells[4],gridCells[5]);
            }
            if ((E & 32) != 0) {
                vertlist[5] = VertexInterp(gridPos[5],gridPos[6],gridCells[5],gridCells[6]);
            }
            if ((E & 64) != 0) {
                vertlist[6] = VertexInterp(gridPos[6],gridPos[7],gridCells[6],gridCells[7]);
            }
            if ((E & 128) != 0) {
                vertlist[7] = VertexInterp(gridPos[7],gridPos[4],gridCells[7],gridCells[4]);
            }
            if ((E & 256) != 0) {
                vertlist[8] = VertexInterp(gridPos[0],gridPos[4],gridCells[0],gridCells[4]);
            }
            if ((E & 512) != 0) {
                vertlist[9] = VertexInterp(gridPos[1],gridPos[5],gridCells[1],gridCells[5]);
            }
            if ((E & 1024) != 0) {
                vertlist[10] = VertexInterp(gridPos[2],gridPos[6],gridCells[2],gridCells[6]);
            }
            if ((E & 2048) != 0) {
                vertlist[11] = VertexInterp(gridPos[3],gridPos[7],gridCells[3],gridCells[7]);
            }

            for (int tCount = 0; triTable[tCount+16*cubeIndex] != -1; tCount+=3)
            {
                uint index = atomicCounterIncrement(triCount);

                //per-triangle normals
                //vec3 v1 = vertlist[triTable[cubeIndex*16+tCount  ]]*chunkStride + chunkPosition;
                //vec3 v2 = vertlist[triTable[cubeIndex*16+tCount+1]]*chunkStride + chunkPosition;
                //vec3 v3 = vertlist[triTable[cubeIndex*16+tCount+2]]*chunkStride + chunkPosition;
                //normals[3*index  ] = vec4(normalize(cross(v2-v1,v3-v1)),0);
                //normals[3*index+1] = vec4(normalize(cross(v2-v1,v3-v1)),0);
                //normals[3*index+2] = vec4(normalize(cross(v2-v1,v3-v1)),0);


                for (int t = 0; t < 3;t++)
                {
                    vec3 vertPos = vertlist[triTable[cubeIndex*16+tCount+t]]*chunkStride + chunkPosition;
                    vertices[3*index+t] = vec4(vertPos,1);//vertPos;

                    //numerical normals
                    //float eps = 0.1;
                    //float dX = (distFunc(vertPos+vec3(eps,0,0))-distFunc(vertPos));
                    //float dY = (distFunc(vertPos+vec3(0,eps,0))-distFunc(vertPos));
                    //float dZ = (distFunc(vertPos+vec3(0,0,eps))-distFunc(vertPos));
                    //normals[3*index+t] = normalize(vec4(dX,dY,dZ,0));

                    //specified normals
                    normals[3*index+t] = vec4(modified_normal(vertPos),0);

                }
			}
		}
	}
}
