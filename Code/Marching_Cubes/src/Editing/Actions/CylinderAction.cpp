#include "CylinderAction.h"

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