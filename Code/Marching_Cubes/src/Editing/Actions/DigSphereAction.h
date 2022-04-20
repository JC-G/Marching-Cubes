#ifndef DIGSPHEREACTION_H
#define DIGSPHEREACTION_H

#include "Action.h"
class DigSphereAction : public Action {
    public:
        DigSphereAction(float r);
        
        void onMouseDown(glm::vec3 pos) override;
        void onMouseUp(glm::vec3 pos) override;
        void onMouseHold(glm::vec3 pos) override;
        void onCancel() override;
        std::string getDescription() override;
        void increaseSize() override;
        void decreaseSize() override;
        void drawPreview() override;
        std::string getDetails() override;

        float radius;
};


#endif