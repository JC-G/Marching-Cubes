#include "DigSphereAction.h"

DigSphereAction::DigSphereAction(float r)
:radius(r)
{

}

void DigSphereAction::onMouseDown(glm::vec3 pos){};
void DigSphereAction::onMouseUp(glm::vec3 pos){};

void DigSphereAction::onMouseHold(glm::vec3 pos){
    Brush* b = new EllipsoidBrush(pos,glm::vec3(radius),MODE_DELETE);
    Editing::allBrushes.push_back(b);
    Editing::newBrushes.push_back(b);
};

void DigSphereAction::onCancel(){};

std::string DigSphereAction::getDescription() {
    return "Dig Sphere, Radius " + std::to_string(radius);
}