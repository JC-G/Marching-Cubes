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


#include "Engine/Camera.h"

class Window
{
    public:
        virtual ~Window();

        //Initialise OpenGL, Creates Window
        static bool initGL();
        static GLFWwindow* window;
        static glm::mat4 getProjectionMatrix();
        static void attachCamera(Camera& cam);
        static void handleInput();
        static const int width = 1920;
        static const int height = 1080;
        static Camera activeCamera;
    protected:

    private:
        Window();

        static double deltaTime();
};

#endif // WINDOW_H
