#ifndef WINDOW_H
#define WINDOW_H
/**
Handles creating window, receiving input, ...
Static class - there is only one window
*/

#include "GL_headers.h"

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
