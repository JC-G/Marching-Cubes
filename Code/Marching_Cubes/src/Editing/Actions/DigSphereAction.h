#ifndef DIGSPHEREACTION_H
#define DIGSPHEREACTION_H

#include "Action.h"
#include "Editing.h"
#include "Brush.h"
class DigSphereAction : public Action {
    public:
        DigSphereAction(float r);
        
        void onMouseDown(glm::vec3 pos) override;
        void onMouseUp(glm::vec3 pos) override;
        void onMouseHold(glm::vec3 pos) override;
        void onCancel() override;
        std::string getDescription() override;

        float radius;
};


#endif