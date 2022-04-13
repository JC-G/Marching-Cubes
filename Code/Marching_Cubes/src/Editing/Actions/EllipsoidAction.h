#ifndef ELLIPSOIDACTION_H
#define ELLIPSOIDACTION_H

#include "Action.h"

class EllipsoidAction : public Action {
    public:
        EllipsoidAction(glm::vec3 r3);
        void onMouseDown(glm::vec3 pos) override;
        void increaseSize() override;
        void decreaseSize() override;

        void handleInput(glm::vec3 placePos) override;

        std::string getDescription() override;

        void drawPreview() override;
    private:
        glm::vec3 r3;
        int editComponent = 0;
};

#endif