#include "Editing.h"
#include "SphereAction.h"
#include "CylinderAction.h"
#include "DigSphereAction.h"
#include "BezierAction.h"
#include "CubicBezierAction.h"
#include "InterpolationSplineAction.h"
#include "RoadAction.h"
#include "SingleSphereAction.h"

#include "EllipsoidBrush.h"

std::vector<Brush*> Editing::allBrushes;
std::vector<Brush*> Editing::newBrushes;
int Editing::actionIndex = 0;
std::vector<Action*> Editing::allActions = {
    new SphereAction(1.0),
    new SingleSphereAction(10.0),
    new CylinderAction(1.0),
    new DigSphereAction(1.0),
    new BezierAction(1.0),
    new CubicBezierAction(1.0),
    new InterpolationSplineAction(1.0),
    new RoadAction(0.1),
};

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
    if (Config::get<std::string>("raycast_mode") == "mesh") {
        float intersectionPoint = O->getIntersectionPoint(pos,direction);
        if (intersectionPoint == std::numeric_limits<float>::max()) {
            return pos; //TODO - temporary fix so that we don't get weird behaviour trying to place a shape infinitely far away
        }
        glm::vec3 p = pos + direction * intersectionPoint;
        return p;
    }

    return pos; //prevent compiler warning
// //At the time of the first marching chunk creation, compile the raycasting shader for later use
//     if (!raycastShader) {
//         raycastShader = &(Shader::ComputeShaderFromVector(std::vector<std::string>{
//             Shader::ReadShaderFile("Shaders/Compute/shadertop.glsl"),
//             Generator->distanceFunction->getShaderCode(),
//             Shader::ReadShaderFile("Shaders/Compute/brush_functions.glsl"),
//             Shader::ReadShaderFile("Shaders/Compute/terrain_modification.glsl"),
            
//         }
//         ));
//     }
}

void Editing::incrementAction() {
    //do not change from cylinder mid-place
    currentAction()->onCancel();
    actionIndex++;
    if (actionIndex >= allActions.size()) {
        actionIndex = 0;
    }
}

Action* Editing::currentAction() {
    return allActions[actionIndex];
}

std::vector<BrushParams> Editing::getBrushesInBox(BrushBoundingBox box) {
    
    std::vector<BrushParams> result;
    for (Brush* b : Editing::allBrushes) {
        if (b->getBoundingBox().intersects(box)) {
            result.push_back(b->getBrushParams());
        }
    }
    return result;
}

