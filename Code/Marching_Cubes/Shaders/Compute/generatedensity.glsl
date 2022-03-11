//GPU Marching Cubes Stage 1 - populate the grid with density function values

layout(binding=1) buffer Grid {
    float densityValues[];
};

//TODO - change this from 8 to ? - maybe better performance?
layout(local_size_x=8,local_size_y=8,local_size_z=8) in;

void generate(uvec3 gid) {
    uint arrID = getArrID(gid);
    densityValues[arrID] = modified_density(gid*chunkStride + chunkPosition);
}

void main() {
	uvec3 gid = gl_GlobalInvocationID;
	if (gid.x > chunkSize.x || gid.y > chunkSize.y || gid.z > chunkSize.z) {
		return;
	}
	generate(gid);
}

