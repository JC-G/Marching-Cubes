//idea: we have a base SDF/normal - which is the generated one
//these are passed into the shader via the density() and normal() functions
//we apply operations to it based on an array of BrushParams operators

float modified_density(vec3 inPos) {
    float terrainDensity = density(inPos);
    float resultDensity = terrainDensity;
    for (int i = 0; i < 50; i++) {
        float testSphere = ellipsoid_density(inPos,vec4(i,10,1,0),vec4(i/10.0,i/3.0,i/10.0,0));
        resultDensity = min(resultDensity,testSphere);
    }
    return resultDensity;
}

vec3 modified_normal(vec3 inPos) {
    int bestI = -1;
    float bestDensity = density(inPos);
    for (int i = 0; i < 50; i++) {
        float testSphere = ellipsoid_density(inPos,vec4(i,10,1,0),vec4(i/10.0,i/3.0,i/10.0,0));
        if (testSphere < bestDensity) {
            bestDensity = testSphere;
            bestI = i;
        }
    }

    if (bestI != -1) {
        return ellipsoid_normal(inPos,vec4(bestI,10,1,0),vec4(bestI/10.0,bestI/3.0,bestI/10.0,0));
    }
    return normal(inPos);
}