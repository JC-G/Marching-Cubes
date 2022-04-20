#include "TestBrushes.h"
#include "EllipsoidBrush.h"
#include "CylinderBrush.h"
#include "Editing.h"

std::vector<Brush*> TestBrushes::randomSpheres;
std::vector<Brush*> TestBrushes::randomCylinders;
void TestBrushes::generateRandomSpheres() {
    if (randomSpheres.size() > 0) {
        //already generated
        return;
    }
    //generate lots of random ellipsoids      
    for (int i = 0; i < 100000; i++) {
        Brush* b = new EllipsoidBrush(
            glm::vec3(randFloat(-500,500),randFloat(0,100),randFloat(-500,500)),
            glm::vec3(randFloat(5,15),randFloat(5,15),randFloat(5,15))
        );
        Editing::placeBrush(b,Window::mainOctree);
    }
}

void TestBrushes::generateRandomCylinders() {
    // if (randomCylinders.size() > 0) {
    //     return;
    // }

    // for (int i = 0; i < 10000; i++) {
    //     glm::vec3 p1(randFloat(-500,500),3,randFloat(-500,500));
    //     glm::vec3 p2 = p1 + glm::vec3(randFloat(-1,1), randFloat(-1,1),randFloat(-1,1));
    //     randomCylinders.push_back(
    //         new CylinderBrush(
    //             p1,p2,randFloat(0.1,0.9)
    //         )
    //     );
    // }
}

float TestBrushes::randFloat(float min, float max) {
    float r01 = float(rand())/float(RAND_MAX);
    return r01 * (max - min) + min;
}