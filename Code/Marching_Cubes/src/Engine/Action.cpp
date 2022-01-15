#include "Action.h"

void Action::handleInput(glm::vec3 placePos) {
    //default input handling for an action
    if (Controller::getKeyState(Window::window,GLFW_KEY_LEFT_BRACKET)) {
        decreaseSize();
    }
    if (Controller::getKeyState(Window::window,GLFW_KEY_RIGHT_BRACKET)) {
        increaseSize();
    }


    if (Controller::mousePressed(Window::window,GLFW_MOUSE_BUTTON_LEFT)) {
        onMouseDown(placePos);
    } else if (Controller::getMouseState(Window::window,GLFW_MOUSE_BUTTON_LEFT)) {
        onMouseHold(placePos);
    } else if (Controller::mouseReleased(Window::window,GLFW_MOUSE_BUTTON_LEFT)) {
        onMouseUp(placePos);
    }
}