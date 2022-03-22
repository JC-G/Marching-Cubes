//idea: we have a base SDF/normal - which is the generated one
//these are passed into the shader via the distance() and normal() functions
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

float modified_distance(vec3 inPos) {
    float terrainDistance = distance(inPos);
    float resultDistance = terrainDistance;
    for (int i = 0; i < brushCount; i++) {
        BrushParams b = brushInstances[i];
        float testDistance = F_MAX;
        if (inBox(b.bottom,b.top,inPos)) {
            if (b.type == 1) {
                testDistance = ellipsoid_distance(inPos,b.location,b.size);
            } else if (b.type == 2) {
                testDistance = cylinder_distance(inPos,b.location,b.size,b.param1);
            } else if (b.type == 3) {
                testDistance = bezier_distance(inPos,b.location,b.size, b.data1, b.param1);
            } else if (b.type == 4) {
                testDistance = cubic_bezier_distance(inPos,b.location,b.size,b.data1,b.data2,b.param1);
            } else if (b.type == 5) {
                testDistance = road_distance(inPos,b.location,b.size,b.data1,b.data2,b.param1);
            }
        }
        if (b.mode == 0) {
            resultDistance = min(resultDistance,testDistance);
        } else if (b.mode == 1) {
            resultDistance = max(resultDistance, -testDistance);
        }
    }
    return resultDistance;
}
vec3 modified_normal(vec3 inPos) {
    int bestI = -1;
    float bestDistance = distance(inPos);

    for (int i = 0; i < brushCount; i++) {
        BrushParams b = brushInstances[i];
        float testDistance = F_MAX;

        if (inBox(b.bottom,b.top,inPos)) {
            if (b.type == 1) {
                testDistance = ellipsoid_distance(inPos,b.location,b.size);
            } else if (b.type == 2) {
                testDistance = cylinder_distance(inPos,b.location,b.size,b.param1);
            } else if (b.type == 3) {
                testDistance = bezier_distance(inPos,b.location,b.size, b.data1, b.param1);
            } else if (b.type == 4) {
                testDistance = cubic_bezier_distance(inPos,b.location,b.size,b.data1,b.data2,b.param1);
            } else if (b.type == 5) {
                testDistance = road_distance(inPos,b.location,b.size,b.data1,b.data2,b.param1);
            }
        }
        if (testDistance < bestDistance && b.mode == 0) {
            bestDistance = testDistance;
            bestI = i;
        } else if(-testDistance > bestDistance && b.mode == 1) {
            bestDistance = -testDistance;
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
        else if (b.type == 4) {
            testNormal = cubic_bezier_normal(inPos,b.location,b.size, b.data1, b.data2, b.param1);
        }
         else if (b.type == 5) {
            testNormal = road_normal(inPos,b.location,b.size,b.data1,b.data2,b.param1);
        }
        if (b.mode == 1) {
            testNormal *= -1.0;
        }
        return testNormal;
    }
    return normal(inPos);
}