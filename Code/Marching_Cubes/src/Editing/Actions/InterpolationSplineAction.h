#ifndef INTERPOLATIONSPLINEACTION_H
#define INTERPOLATIONSPLINEACTION_H

#include "Action.h"
#include <vector>

class InterpolationSplineAction : public Action {
    public:
        InterpolationSplineAction(float r);
        void handleInput(glm::vec3 inPos) override;
        void onMouseDown(glm::vec3 inPos) override;
        void onCancel() override;

        std::string getDescription() override;
        void increaseSize() override;
        void decreaseSize() override;
        void drawPreview() override;

        void finishSpline();

    private:
        float radius;
        std::vector<glm::vec3> points;

};

#endif