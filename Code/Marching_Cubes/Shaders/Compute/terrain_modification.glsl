//idea: we have a base SDF/normal - which is the generated one
//these are passed into the shader via the density() and normal() functions
//we apply operations to it based on an array of BrushParams operators

struct BrushParams {
    vec4 location;
    vec4 size;

    int type;
    int mode;
    float param1;
    float param2;
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
        float testDensity = 0.0;
        if (b.type == 1) {
            testDensity = ellipsoid_density(inPos,b.location,b.size);
        } else if (b.type == 2) {
            testDensity = cylinder_density(inPos,b.location,b.size,b.param1);
        }
        resultDensity = min(resultDensity,testDensity);
    }
    return resultDensity;
}

vec3 modified_normal(vec3 inPos) {
    int bestI = -1;
    float bestDensity = density(inPos);

    for (int i = 0; i < brushCount; i++) {
        BrushParams b = brushInstances[i];
        float testDensity = 0.0;
        if (b.type == 1) {
            testDensity = ellipsoid_density(inPos,b.location,b.size);
        }
        else if (b.type == 2) {
            testDensity = cylinder_density(inPos,b.location,b.size,b.param1);
        }
        if (testDensity < bestDensity) {
            bestDensity = testDensity;
            bestI = i;
        }
    }

    if (bestI != -1) {
        BrushParams b = brushInstances[bestI];        
        vec3 testNormal = vec3(0.0);
        if (b.type == 1) {
            testNormal = ellipsoid_normal(inPos,b.location,b.size);
        }
        else if (b.type == 2) {
            testNormal = cylinder_normal(inPos,b.location,b.size,b.param1);
        }
        return testNormal;
    }
    return normal(inPos);
}