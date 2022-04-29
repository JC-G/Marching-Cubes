#ifndef DIGSINGLESPHEREACTION_H
#define DIGSINGLESPHEREACTION_H

#include "DigSphereAction.h"
class DigSingleSphereAction : public DigSphereAction {
    public:
        DigSingleSphereAction(float r);
        void onMouseDown(glm::vec3 pos) override;
        void onMouseHold(glm::vec3 pos) override;
        std::string getDescription() override;
        std::string getDetails() override;
};

#endif
