#include "TestBrushes.h"
#include "EllipsoidBrush.h"

std::vector<Brush*> TestBrushes::randomSpheres;
void TestBrushes::generateRandomSpheres() {
    if (randomSpheres.size() > 0) {
        //already generated
        return;
    }
    //generate lots of random ellipsoids
    for (int i = 0; i < 10000; i++) {
        randomSpheres.push_back(
            new EllipsoidBrush(
                glm::vec3(randFloat(-500,500),5,randFloat(-500,500)),
                glm::vec3(randFloat(1,10),randFloat(1,10),randFloat(1,10))
            )
        );
    }
}

float TestBrushes::randFloat(float min, float max) {
    float r01 = float(rand())/float(RAND_MAX);
    return r01 * (max - min) + min;
}