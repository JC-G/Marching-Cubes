#include "Editing.h"

std::vector<Brush*> Editing::allBrushes;
std::vector<Brush*> Editing::newBrushes;
float Editing::cr;
glm::vec3 Editing::cp1;
glm::vec3 Editing::cp2;
bool Editing::placing = false;

void Editing::placeSphere(glm::vec3 pos, float r) {
    Brush* b = new EllipsoidBrush(pos,glm::vec3(r));
    allBrushes.push_back(b);
    newBrushes.push_back(b);
}


void Editing::beginCylinder(glm::vec3 pos, float r) {
    if (placing) {
        return;
    }
    placing = true;
    cp1 = pos;
    cr = r;
}

void Editing::endCylinder(glm::vec3 pos) {
    if (!placing) {
        return;
    }
    cp2 = pos;

    Brush* b = new CylinderBrush(cp1,cp2,cr);
    allBrushes.push_back(b);
    newBrushes.push_back(b);

    placing = false;
}  

void Editing::sphereRing(glm::vec3 pos, float ringR, int ringN, float r) {

    for (int i = 0; i < ringN; i++) {
        float theta = float(i)*glm::two_pi<float>()/float(ringN);
        glm::vec3 posi = pos + ringR * glm::vec3(cos(theta),0,sin(theta));
        Brush* b = new EllipsoidBrush(posi,glm::vec3(r));
        allBrushes.push_back(b);
        newBrushes.push_back(b);
    }
}

glm::vec3 Editing::rayCast(glm::vec3 pos, glm::vec3 direction, Octree* O) {
    glm::vec3 p = pos + direction * O->getIntersectionPoint(pos,direction);
    return p;
}


