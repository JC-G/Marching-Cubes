#ifndef DIGCYLINDERACTION_H
#define DIGCYLINDERACTION_H

#include "CylinderAction.h"
class DigCylinderAction : public CylinderAction {
    public:
        DigCylinderAction(float r);
        void onMouseUp(glm::vec3 pos) override;
        std::string getDescription() override;
        std::string getDetails() override;

};

#endif