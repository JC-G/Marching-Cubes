#ifndef BEZIERACTION_H
#define BEZIERACTION_H

#include "Action.h"

class BezierAction : public Action {
    public:
        BezierAction(float r);

        void onMouseDown(glm::vec3 pos) override;
        void onCancel() override;

        std::string getDescription() override;
        void increaseSize() override;
        void decreaseSize() override;
        void drawPreview() override;

        float radius;
        int placingStep = 0;
    private:
        glm::vec3 A;
        glm::vec3 B;
        glm::vec3 C;

};

#endif