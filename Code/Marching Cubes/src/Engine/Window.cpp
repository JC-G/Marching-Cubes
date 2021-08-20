#include "Window.h"

//initialise static member
GLFWwindow* Window::window = NULL;

Window::Window()
{
    //ctor
}

Window::~Window()
{
    //dtor
}

bool Window::initGL()
{
    if (!glfwInit()) {
        return false;
    }

    // open a Window::window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //set a specific version of OpenGL

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glfwWindowHint(GLFW_REFRESH_RATE,60);
    glfwWindowHint(GLFW_VISIBLE,GLFW_TRUE);

    Window::window = glfwCreateWindow(640,480,"Window Title",NULL,NULL);
//    if (Window::window == NULL) {
//        printf("Window was not created");
//        glfwTerminate();
//        return false;
//    }
    glfwMakeContextCurrent(Window::window);

    //set input mode
//    glfwSetInputMode(Window::window,GLFW_STICKY_KEYS,GL_TRUE);
//    glfwSetInputMode(Window::window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);


    //initialise GLEW
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        printf("GLEW was not loaded");
        return false;

    }

    //misc OpenGL Setup
    glClearColor(0,148./255,1,1);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

glm::mat4 Window::getProjectionMatrix()
{
    return glm::perspective(glm::radians(90.0f),640.0f/480.0f,0.1f,10000.0f);
}
