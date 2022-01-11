#ifndef EDITING_H
#define EDITING_H

#include <vector>
#include "Brush.h"
#include "GL_headers.h"
#include "EllipsoidBrush.h"
#include "CylinderBrush.h"
#include "Octree.h"
#include "Shader.h"
#include "Action.h"
#include "Actions/SphereAction.h"
#include "Actions/CylinderAction.h"
#include "Actions/DigSphereAction.h"
#include "BezierBrush.h"
#include "Actions/BezierAction.h"
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
    private:
        static Shader* raycastShader;
        static int actionIndex;
};

#endif