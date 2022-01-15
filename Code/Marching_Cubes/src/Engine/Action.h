#ifndef ACTION_H
#define ACTION_H

#include "GL_headers.h"
#include "Controller.h"

class Action {
    public:
        virtual void onMouseDown(glm::vec3 pos) {};
        virtual void onMouseUp(glm::vec3 pos) {};
        virtual void onMouseHold(glm::vec3 pos) {};
        virtual void onCancel() {};
        virtual std::string getDescription() {return "";};
        virtual void increaseSize() {};
        virtual void decreaseSize() {};

        virtual void drawPreview() {};

        virtual void handleInput(glm::vec3 placePos);
};

#endif