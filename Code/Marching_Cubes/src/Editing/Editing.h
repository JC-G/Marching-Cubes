#ifndef EDITING_H
#define EDITING_H

#include <vector>

#include "Octree.h"
#include "Brush.h"
#include "Action.h"
#include "Shader.h"
//forward declaration of octree to defeat circular reference
class Octree;
class Editing {
    public:
        static std::vector<Brush*> allBrushes;
        static std::vector<Brush*> newBrushes;

        static void sphereRing(glm::vec3 pos, float ringR, int ringN, float r);

        static glm::vec3 rayCast(glm::vec3 pos, glm::vec3 direction, Octree* O);

        static std::vector<Action*> allActions;

        static void incrementAction();
        static Action* currentAction();
        // static std::vector<BrushParams> getBrushesInBox(BrushBoundingBox box);

        static void placeBrush(Brush* b, Octree* O);
    private:
        static Shader* raycastShader;
        static int actionIndex;
};

#endif