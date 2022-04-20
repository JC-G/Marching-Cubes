//Transvoxel Stage 1 - populate the grid

layout(binding = 1) buffer Grid {
    float distanceValues[];
};

// layout(binding = 4) uniform atomic_uint has_negative;
// layout(binding = 5) uniform atomic_uint has_positive;

//TODO - change this from 8 to ? - maybe better performance?
layout(local_size_x=8,local_size_y=8,local_size_z=8) in;

void generate(uvec3 gid, uvec3 halfXYZ) {
    uint arrID = getArrID(gid, halfXYZ);
    float value = modified_distance((gid + 0.5 * vec3(halfXYZ)) * chunkStride + chunkPosition);
    // if (value < 0) {
    //     atomicCounterIncrement(has_negative);
    // }
    // if (value >= 0) {
    //     atomicCounterIncrement(has_positive);
    // }
    distanceValues[arrID] = value;
}

void main() {
    uvec3 gid = gl_GlobalInvocationID;
    if (gid.x > chunkSize.x || gid.y > chunkSize.y || gid.z > chunkSize.z) {
		return;
	}
	generate(gid,uvec3(0));

	if ((gid.x == 0 && (edgeIndex & 1) != 0) || (gid.x == chunkSize.x && (edgeIndex & 2) != 0)) {
        if (gid.y < chunkSize.y) {
            generate(gid,uvec3(0,1,0));
        }
        if (gid.z < chunkSize.z) {
            generate(gid,uvec3(0,0,1));
        }
        if (gid.y < chunkSize.y && gid.z < chunkSize.z) {
            generate(gid,uvec3(0,1,1));
        }
    }

    if ((gid.y == 0 && (edgeIndex & 4) != 0) || (gid.y == chunkSize.y && (edgeIndex & 8) != 0)) {
        if (gid.x < chunkSize.x) {
            generate(gid,uvec3(1,0,0));
        }
        if (gid.z < chunkSize.z) {
            generate(gid,uvec3(0,0,1));
        }
        if (gid.x < chunkSize.x && gid.z < chunkSize.z) {
            generate(gid,uvec3(1,0,1));
        }
    }

    if ((gid.z == 0 && (edgeIndex & 16) != 0) || (gid.z == chunkSize.z && (edgeIndex & 32) != 0)) {
        if (gid.x < chunkSize.x) {
            generate(gid,uvec3(1,0,0));
        }
        if (gid.y < chunkSize.y) {
            generate(gid,uvec3(0,1,0));
        }
        if (gid.x < chunkSize.x && gid.y < chunkSize.y) {
            generate(gid,uvec3(1,1,0));
        }
    }
}


