#ifndef SINGLESPHEREACTION_H
#define SINGLESPHEREACTION_H

#include "SphereAction.h"

class SingleSphereAction : public SphereAction {
    public:
        SingleSphereAction(float r);
        void onMouseDown(glm::vec3 pos) override;
        void onMouseHold(glm::vec3 pos) override;
        std::string getDescription() override;
        std::string getDetails() override;
        
};

#endif