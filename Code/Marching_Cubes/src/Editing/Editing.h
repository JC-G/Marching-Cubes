#ifndef EDITING_H
#define EDITING_H

#include <vector>
#include "Brush.h"
#include "GL_headers.h"
#include "EllipsoidBrush.h"
#include "CylinderBrush.h"
#include "Octree.h"
#include "Shader.h"
//forward declaration of octree to defeat circular reference
class Octree;
class Editing {
    public:
        static std::vector<Brush*> allBrushes;
        static std::vector<Brush*> newBrushes;
        static void placeSphere(glm::vec3 pos, float r);
        static void digSphere(glm::vec3 pos, float r);

        static void beginCylinder(glm::vec3 pos, float r);
        static void endCylinder(glm::vec3 pos);

        static void sphereRing(glm::vec3 pos, float ringR, int ringN, float r);

        static glm::vec3 rayCast(glm::vec3 pos, glm::vec3 direction, Octree* O);
    private:
        
        static bool placing;
        static glm::vec3 cp1;
        static glm::vec3 cp2;
        static float cr;

        static Shader* raycastShader;
};

#endif