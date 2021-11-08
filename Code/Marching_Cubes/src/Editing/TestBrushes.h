#ifndef TESTBRUSHES_H
#define TESTBRUSHES_H

#include "Brush.h"
#include <vector>
#include <random>
class TestBrushes {
    public:
        static std::vector<Brush*> randomSpheres;
        static void generateRandomSpheres();
        static float randFloat(float min, float max);
};


#endif // TESTBRUSHES_H