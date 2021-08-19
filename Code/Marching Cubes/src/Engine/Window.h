#ifndef WINDOW_H
#define WINDOW_H
#define GLEW_STATIC
/**
Handles creating window, receiving input, ...
Static class - there is only one window
*/


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

class Window
{
    public:
        virtual ~Window();

        //Initialise OpenGL, Creates Window
        static bool initGL();
        static GLFWwindow* window;
    protected:

    private:
        Window();
};

#endif // WINDOW_H
