#include "SphereAction.h"

#include "Editing.h"
#include "Preview.h"
#include "Window.h"
#include "EllipsoidBrush.h"

SphereAction::SphereAction(float r)
:radius(r)
{

}

void SphereAction::onMouseDown(glm::vec3 pos){};
void SphereAction::onMouseUp(glm::vec3 pos){};

void SphereAction::onMouseHold(glm::vec3 pos){
    Brush* b = new EllipsoidBrush(pos,glm::vec3(radius));
    Editing::allBrushes.push_back(b);
    Editing::newBrushes.push_back(b);
};

void SphereAction::onCancel(){};

std::string SphereAction::getDescription() {
    return "Sphere, Radius " + std::to_string(radius);
}

void SphereAction::increaseSize() {
    radius += 0.01;
}
void SphereAction::decreaseSize() {
    radius -= 0.01;
}

void SphereAction::drawPreview() {
    Preview::drawPreviewSphere(glm::vec3(radius),Window::placePos);
}