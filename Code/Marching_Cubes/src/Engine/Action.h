#ifndef ACTION_H
#define ACTION_H

#include "GL_headers.h"

// #include "Window.h"

class Action {
    public:
        virtual void onMouseDown(glm::vec3 pos) = 0;
        virtual void onMouseUp(glm::vec3 pos) = 0;
        virtual void onMouseHold(glm::vec3 pos) = 0;
        virtual void onCancel() = 0;
        virtual std::string getDescription() = 0;
        virtual void increaseSize() = 0;
        virtual void decreaseSize() = 0;

        virtual void drawPreview() = 0;
};

#endif