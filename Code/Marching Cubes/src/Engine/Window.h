#ifndef WINDOW_H
#define WINDOW_H
/**
Handles creating window, receiving input, ...
Static class - there is only one window
*/

#include "GL_headers.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

class Window
{
    public:
        virtual ~Window();

        //Initialise OpenGL, Creates Window
        static bool initGL();
        static GLFWwindow* window;
        static glm::mat4 getProjectionMatrix();
    protected:

    private:
        Window();
};

#endif // WINDOW_H
