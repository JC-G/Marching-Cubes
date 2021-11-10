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
#include "Config.h"

#include "Engine/Camera.h"
#include "Editing.h"

class Window
{
    public:
        virtual ~Window();

        //Initialise OpenGL, Creates Window
        static bool initGL();
        static GLFWwindow* window;
        static glm::mat4 getProjectionMatrix();
        static void attachCamera(Camera* cam);
        static void handleInput();
        static int width;
        static int height;
        static Camera* activeCamera;
    protected:

    private:
        Window();

        static double deltaTime();
};

#endif // WINDOW_H
