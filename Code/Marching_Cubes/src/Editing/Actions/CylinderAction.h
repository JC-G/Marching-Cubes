#ifndef CYLINDERACTION_H
#define CYLINDERACTION_H

#include "Action.h"
#include "Editing.h"

class CylinderAction : public Action {
    public:
        CylinderAction(float r);

        void onMouseDown(glm::vec3 pos) override;
        void onMouseUp(glm::vec3 pos) override;
        void onMouseHold(glm::vec3 pos) override;
        void onCancel() override;
        std::string getDescription() override;
        void increaseSize() override;
        void decreaseSize() override;
        void drawPreview() override;

        float radius;
        bool placing = false;
    private:
        glm::vec3 pos1;
        glm::vec3 pos2;
};

#endif