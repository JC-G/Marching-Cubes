#include "EllipsoidAction.h"

#include "EllipsoidBrush.h"
#include "Editing.h"
#include "Preview.h"
EllipsoidAction::EllipsoidAction(glm::vec3 r3) :r3(r3){};

void EllipsoidAction::onMouseDown(glm::vec3 pos) {
    Brush* b = new EllipsoidBrush(pos,r3);
    Editing::allBrushes.push_back(b);
    Editing::newBrushes.push_back(b);
}

std::string EllipsoidAction::getDescription() {
    return "Ellipsoid: " + glm::to_string(r3) + ", Editing Component (" + "XYZ"[editComponent] + ")";
}


void EllipsoidAction::increaseSize() {
    r3[editComponent] += 0.01;
}
void EllipsoidAction::decreaseSize() {
    r3[editComponent] -= 0.01;

}

void EllipsoidAction::handleInput(glm::vec3 placePos) {
    Action::handleInput(placePos);
    
    if (Controller::keyPressed(Window::window,GLFW_KEY_APOSTROPHE)) {
        editComponent = (editComponent + 1) % 3;
    }
    
}


void EllipsoidAction::drawPreview() {
    Preview::drawPreviewSphere(r3,Window::placePos);
    
}

std::string EllipsoidAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Size in Direction\n"
           "(]) Increase Size in Direction\n"
           "(') Change Modified Direction\n"
           "(Click) Place Ellipsoid";
}