#include "Controller.h"

std::map<int,int> Controller::keyStates;
std::map<int,int> Controller::mouseStates;
std::map<int,bool> Controller::heldKeys;
std::map<int,bool> Controller::heldButtons;

bool Controller::keyPressed(GLFWwindow* window, int key) {
    int newState = glfwGetKey(window, key);
    if (newState == GLFW_PRESS && keyStates[key] != GLFW_PRESS) {
        heldKeys[key] = true;
        keyStates[key] = GLFW_PRESS;
        return true;
    } else if (newState != GLFW_PRESS) {
        keyStates[key] = GLFW_RELEASE;
    }
    return false;
}

bool Controller::keyReleased(GLFWwindow* window, int key) {
    int newState = glfwGetKey(window, key);
    if (newState == GLFW_RELEASE && heldKeys[key]) {
        heldKeys[key] = false;
        return true;
    } 
    return false;
}

int Controller::getKeyState(GLFWwindow* window, int key) {
    int state = glfwGetKey(window, key);
    // if (keyStates.find(key) == keyStates.end()) {
    //     keyStates[key] = state;
    // }
    return state;
}

bool Controller::mousePressed(GLFWwindow* window, int button) {
    int newState = glfwGetMouseButton(window, button);
    if (newState == GLFW_PRESS && mouseStates[button] != GLFW_PRESS) {
        heldButtons[button] = true;
        mouseStates[button] = GLFW_PRESS;
        return true;
    } else if (newState != GLFW_PRESS) {
        mouseStates[button] = GLFW_RELEASE;
    }
    return false;
}

bool Controller::mouseReleased(GLFWwindow* window, int button) {
    int newState = glfwGetMouseButton(window, button);
    if (newState == GLFW_RELEASE && heldButtons[button]) {
        heldButtons[button] = false;
        return true;
    }
    return false;
}

bool Controller::getMouseState(GLFWwindow* window, int button) {
    int state = glfwGetMouseButton(window,button);
    return state;
}