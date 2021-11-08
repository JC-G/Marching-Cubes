#ifndef TESTBRUSHES_H
#define TESTBRUSHES_H

#include "Brush.h"
#include <vector>
#include <random>
class TestBrushes {
    public:
        static std::vector<Brush*> randomSpheres;
        static std::vector<Brush*> randomCylinders;
        static void generateRandomSpheres();
        static void generateRandomCylinders();
        static float randFloat(float min, float max);
};


#endif // TESTBRUSHES_H