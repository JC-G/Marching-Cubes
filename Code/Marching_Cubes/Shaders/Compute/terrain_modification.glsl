//idea: we have a base SDF/normal - which is the generated one
//these are passed into the shader via the density() and normal() functions
//we apply operations to it based on an array of BrushParams operators

struct BrushParams {
    vec4 location;
    vec4 size;

    int type;
    int mode;
    int param1;
    int param2;
};
// bindings 0-15 are used by the standard transvoxel algorithm, including constant tables
layout (std140, binding = 16) buffer BrushList {
    BrushParams brushInstances[];
};

uniform int brushCount;

float modified_density(vec3 inPos) {
    float terrainDensity = density(inPos);
    float resultDensity = terrainDensity;
    for (int i = 0; i < brushCount; i++) {
        BrushParams b = brushInstances[i];
        float testSphere = ellipsoid_density(inPos,b.location,b.size);
        resultDensity = min(resultDensity,testSphere);
    }
    // for (int i = 0; i < 50; i++) {
    //     float testSphere = ellipsoid_density(inPos,vec4(i,10,1,0),vec4(i/10.0,i/3.0,i/10.0,0));
    //     resultDensity = min(resultDensity,testSphere);
    // }
    return resultDensity;
}

vec3 modified_normal(vec3 inPos) {
    int bestI = -1;
    float bestDensity = density(inPos);

    for (int i = 0; i < brushCount; i++) {
        BrushParams b = brushInstances[i];
        float testSphere = ellipsoid_density(inPos,b.location,b.size);
        if (testSphere < bestDensity) {
            bestDensity = testSphere;
            bestI = i;
        }
    }
    // for (int i = 0; i < 50; i++) {
    //     float testSphere = ellipsoid_density(inPos,vec4(i,10,1,0),vec4(i/10.0,i/3.0,i/10.0,0));
    //     if (testSphere < bestDensity) {
    //         bestDensity = testSphere;
    //         bestI = i;
    //     }
    // }

    if (bestI != -1) {
        return ellipsoid_normal(inPos,brushInstances[bestI].location,brushInstances[bestI].size);
    }
    return normal(inPos);
}