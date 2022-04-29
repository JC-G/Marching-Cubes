#include "DigCylinderAction.h"

#include "CylinderBrush.h"
#include "Editing.h"
DigCylinderAction::DigCylinderAction(float r) : CylinderAction(r){};
void DigCylinderAction::onMouseUp(glm::vec3 pos){
    if (!placing) {
        return;
    }
    placing = false;
    pos2 = pos;
    
    Brush* b = new CylinderBrush(pos1,pos2,radius);
    Editing::placeBrush(b,Window::mainOctree);

};

std::string DigCylinderAction::getDescription() {
    return "Dig Cylinder, Radius " +  std::to_string(radius);
}

std::string DigCylinderAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Radius\n"
           "(]) Increase Radius\n"
           "(Click and Drag) Dig Cylinder";
}