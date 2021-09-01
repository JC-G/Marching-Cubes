#include "Window.h"

//initialise static members
GLFWwindow* Window::window = NULL;
Camera Window::activeCamera;

Window::Window()
{
    //ctor
}

Window::~Window()
{
    //dtor
}
double Window::deltaTime()
{
    double dt = glfwGetTime();
    glfwSetTime(0);
    return dt;
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

    Window::window = glfwCreateWindow(Window::width,Window::height,"Window Title",NULL,NULL);
    if (Window::window == NULL) {
        printf("Window was not created");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(Window::window);

    //set input mode
    glfwSetInputMode(Window::window,GLFW_STICKY_KEYS,GL_TRUE);
    glfwSetInputMode(Window::window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window,width/2,height/2);


    //initialise GLEW
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        printf("GLEW was not loaded");
        return false;

    }

    //misc OpenGL Setup
    glClearColor(0,148./255,1,1);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

glm::mat4 Window::getProjectionMatrix()
{
    return glm::perspective(glm::radians(90.0f),(float)(Window::width)/(float)(Window::height),0.1f,10000.0f);
}


void Window::attachCamera(Camera& cam)
{
    Window::activeCamera = cam;
}
void Window::handleInput()
{
    //camera rotation
    double X,Y;
    glfwGetCursorPos(window,&X,&Y);
    glfwSetCursorPos(window,width/2,height/2);
    double dX = width/2-X;
    double dY = height/2-Y;

    double dt = deltaTime();

    activeCamera.rotateFromMouse(dX,dY,dt);

    //movement
    // vec3(fwd/back, up/down, left/right)
    glm::vec3 movement(0.0f);
    if (glfwGetKey(window,GLFW_KEY_W)) {
        movement.x+=1.0f;
    }

    if (glfwGetKey(window,GLFW_KEY_S)) {
        movement.x-=1.0f;
    }

    if (glfwGetKey(window,GLFW_KEY_D)) {
        movement.z+=1.0f;
    }

    if (glfwGetKey(window,GLFW_KEY_A)) {
        movement.z-=1.0f;
    }

    if (glfwGetKey(window,GLFW_KEY_SPACE)) {
        movement.y+=1.0f;
    }

    if (glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)) {
        movement.y-=1.0f;
    }

    activeCamera.moveFromVec3(movement,dt);



}
