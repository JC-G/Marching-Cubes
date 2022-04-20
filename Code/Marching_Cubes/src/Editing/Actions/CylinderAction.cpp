#include "CylinderAction.h"

#include "Editing.h"
#include "Preview.h"
#include "Window.h"
#include "CylinderBrush.h"

CylinderAction::CylinderAction(float r)
:radius(r)
{

}

void CylinderAction::onMouseDown(glm::vec3 pos){
    if (placing) {
        return;
    }
    placing = true;
    pos1 = pos;
};
void CylinderAction::onMouseUp(glm::vec3 pos){
    if (!placing) {
        return;
    }
    placing = false;
    pos2 = pos;
    
    Brush* b = new CylinderBrush(pos1,pos2,radius);
    Editing::allBrushes.push_back(b);
    Editing::newBrushes.push_back(b);

};
void CylinderAction::onMouseHold(glm::vec3 pos){};

void CylinderAction::onCancel(){
    placing = false;
}

std::string CylinderAction::getDescription() {
    return "Cylinder, Radius " +  std::to_string(radius);
}

void CylinderAction::increaseSize() {
    if (placing) {
        return;
    }
    radius += 0.01;
}
void CylinderAction::decreaseSize() {
    if (placing) {
        return;
    }
    radius -= 0.01;
}

void CylinderAction::drawPreview() {
    if (!placing) {
        return;
    }
    Preview::drawPreviewCylinder(radius,pos1,Window::placePos);
}

std::string CylinderAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Radius\n"
           "(]) Increase Radius\n"
           "(Click and Drag) Place Cylinder";
}