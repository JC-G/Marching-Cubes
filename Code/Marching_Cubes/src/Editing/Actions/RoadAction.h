#ifndef ROADACTION_H
#define ROADACTION_H

#include "Action.h"
#include <vector>

class RoadAction : public Action {
       public:
        RoadAction(float r);
        void handleInput(glm::vec3 inPos) override;
        void onMouseDown(glm::vec3 inPos) override;
        void onCancel() override;

        std::string getDescription() override;
        void increaseSize() override;
        void decreaseSize() override;
        void drawPreview() override;

        void finishSpline();

        std::string getDetails() override;

    private:
        float radius;
        std::vector<glm::vec3> points;

}; 
#endif