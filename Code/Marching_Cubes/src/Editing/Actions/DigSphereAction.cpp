#include "DigSphereAction.h"

#include "Editing.h"
#include "Preview.h"
#include "Window.h"
#include "EllipsoidBrush.h"

DigSphereAction::DigSphereAction(float r)
:radius(r)
{

}

void DigSphereAction::onMouseDown(glm::vec3 pos){};
void DigSphereAction::onMouseUp(glm::vec3 pos){};

void DigSphereAction::onMouseHold(glm::vec3 pos){
    Brush* b = new EllipsoidBrush(pos,glm::vec3(radius),MODE_DELETE);
    Editing::placeBrush(b,Window::mainOctree);
};

void DigSphereAction::onCancel(){};

std::string DigSphereAction::getDescription() {
    return "Dig Sphere, Radius " + std::to_string(radius);
}


void DigSphereAction::increaseSize() {
    radius += 0.01;
}
void DigSphereAction::decreaseSize() {
    radius -= 0.01;
}

void DigSphereAction::drawPreview() {
    Preview::drawPreviewSphere(glm::vec3(radius),Window::placePos);
}


std::string DigSphereAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Radius\n"
           "(]) Increase Radius\n"
           "(Click) Dig Sphere";
}