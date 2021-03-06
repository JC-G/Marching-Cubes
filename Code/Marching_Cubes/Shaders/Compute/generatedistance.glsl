//GPU Marching Cubes Stage 1 - populate the grid with distance function values

layout(binding=1) buffer Grid {
    float distanceValues[];
};

//TODO - change this from 8 to ? - maybe better performance?
layout(local_size_x=8,local_size_y=8,local_size_z=8) in;

void generate(uvec3 gid) {
    uint arrID = getArrID(gid); //get the index into the buffer
    distanceValues[arrID] = modified_distance(gid*chunkStride + chunkPosition); //call the SDF at the corresponding world-space location
}

void main() {
	uvec3 gid = gl_GlobalInvocationID;
	if (gid.x > chunkSize.x || gid.y > chunkSize.y || gid.z > chunkSize.z) {
		return; //do not try to compute the SDF if gid is outside of the volume
	}
	generate(gid);
}

