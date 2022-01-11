//idea: we have a base SDF/normal - which is the generated one
//these are passed into the shader via the density() and normal() functions
//we apply operations to it based on an array of BrushParams operators

struct BrushParams {
    vec4 location;
    vec4 size;

    vec4 bottom;
    vec4 top;

    int type;
    int mode;
    float param1;
    float param2;
    vec4 data1;
    vec4 data2;
};
// bindings 0-15 are used by the standard transvoxel algorithm, including constant tables
layout (std140, binding = 16) buffer BrushList {
    BrushParams brushInstances[];
};

uniform int brushCount;

bool inBox(vec4 bottom, vec4 top, vec3 inPos) {
    return all(lessThanEqual(bottom.xyz,inPos)) && all(lessThanEqual(inPos,top.xyz));
}

const float F_MAX = 1e20;

float modified_density(vec3 inPos) {
    float terrainDensity = density(inPos);
    float resultDensity = terrainDensity;
    for (int i = 0; i < brushCount; i++) {
        BrushParams b = brushInstances[i];
        float testDensity = F_MAX;
        if (inBox(b.bottom,b.top,inPos)) {
            if (b.type == 1) {
                testDensity = ellipsoid_density(inPos,b.location,b.size);
            } else if (b.type == 2) {
                testDensity = cylinder_density(inPos,b.location,b.size,b.param1);
            } else if (b.type == 3) {
                testDensity = bezier_density(inPos,b.location,b.size, b.data1, b.param1);
            }
        }
        if (b.mode == 0) {
            resultDensity = min(resultDensity,testDensity);
        } else if (b.mode == 1) {
            resultDensity = max(resultDensity, -testDensity);
        }
    }
    return resultDensity;
}
vec3 modified_normal(vec3 inPos) {
    int bestI = -1;
    float bestDensity = density(inPos);

    for (int i = 0; i < brushCount; i++) {
        BrushParams b = brushInstances[i];
        float testDensity = F_MAX;

        if (inBox(b.bottom,b.top,inPos)) {
            if (b.type == 1) {
                testDensity = ellipsoid_density(inPos,b.location,b.size);
            }
            else if (b.type == 2) {
                testDensity = cylinder_density(inPos,b.location,b.size,b.param1);
            } else if (b.type == 3) {
                testDensity = bezier_density(inPos,b.location,b.size, b.data1, b.param1);
            }
        }
        if (testDensity < bestDensity && b.mode == 0) {
            bestDensity = testDensity;
            bestI = i;
        } else if(-testDensity > bestDensity && b.mode == 1) {
            bestDensity = -testDensity;
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
        else if (b.type == 3) {
            testNormal = bezier_normal(inPos,b.location,b.size, b.data1, b.param1);
        }
        if (b.mode == 1) {
            testNormal *= -1.0;
        }
        return testNormal;
    }
    return normal(inPos);
}