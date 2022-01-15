#ifndef SPHEREACTION_H
#define SPHEREACTION_H

#include "Action.h"

class SphereAction : public Action {
    public:
        SphereAction(float r);
        
        void onMouseDown(glm::vec3 pos) override;
        void onMouseUp(glm::vec3 pos) override;
        void onMouseHold(glm::vec3 pos) override;
        void onCancel() override;
        std::string getDescription() override;
        void increaseSize() override;
        void decreaseSize() override;
        void drawPreview() override;

        float radius;
};


#endif