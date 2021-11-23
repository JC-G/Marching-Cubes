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
#include "Controller.h"

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

        //Basic OpenGL Error Printing: https://learnopengl.com/In-Practice/Debugging
        static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                             GLsizei length, const char *message, const void *userParam);
        static Octree* mainOctree;
    protected:

    private:
        Window();

        static double deltaTime();
};

#endif // WINDOW_H
