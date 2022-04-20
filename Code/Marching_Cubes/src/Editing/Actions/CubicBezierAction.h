#ifndef CUBICBEZIERACTION_H
#define CUBICBEZIERACTION_H

#include "Action.h"

class CubicBezierAction : public Action {
    public:
        CubicBezierAction(float r);
        void onMouseDown(glm::vec3 pos) override;
        void onCancel() override;

        std::string getDescription() override;
        void increaseSize() override;
        void decreaseSize() override;
        void drawPreview() override;

        float radius;
        int placingStep = 0;

        std::string getDetails() override;

    private:
        glm::vec3 A;
        glm::vec3 B;
        glm::vec3 C;
        glm::vec3 D;
};

#endif