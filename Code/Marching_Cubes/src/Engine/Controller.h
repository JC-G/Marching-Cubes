#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Window.h"
#include <map>

class Controller
{
    public:
        static bool keyPressed(GLFWwindow* window, int key);
        static bool keyReleased(GLFWwindow* window, int key);
        static int getKeyState(GLFWwindow* window, int key);

        static bool mousePressed(GLFWwindow* window, int button);
        static bool mouseReleased(GLFWwindow* window, int button);
        static bool getMouseState(GLFWwindow* window, int button);
    
    private:
        static std::map<int,int> keyStates;
        static std::map<int,int> mouseStates;

        static std::map<int,bool> heldKeys;
        static std::map<int,bool> heldButtons;
};

#endif //CONTROLLER_H