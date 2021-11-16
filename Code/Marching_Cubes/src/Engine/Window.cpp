#include "Window.h"

//initialise static members
GLFWwindow* Window::window = NULL;
Camera* Window::activeCamera;
int Window::width;
int Window::height;
Octree* Window::mainOctree;
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
    width = Config::get<int>("screen_width");
    height = Config::get<int>("screen_height");
    // open a Window::window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //set a specific version of OpenGL

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glfwWindowHint(GLFW_REFRESH_RATE,60);
    glfwWindowHint(GLFW_VISIBLE,GLFW_TRUE);

    if (Config::get<bool>("opengl_debug")) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    }

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

    
    if (Config::get<bool>("opengl_debug")) {
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            std::cout << "Enabling OpenGL Debug" << std::endl;
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
            glDebugMessageControl(GL_DEBUG_SOURCE_API, 
                      GL_DEBUG_TYPE_ERROR, 
                      GL_DEBUG_SEVERITY_HIGH,
                      0, nullptr, GL_TRUE); 
        }
    }

    //misc OpenGL Setup
    glClearColor(0,148./255,1,1);
    if (Config::get<bool>("backface_culling")) {
        glEnable(GL_CULL_FACE);
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

glm::mat4 Window::getProjectionMatrix()
{
    return glm::perspective(glm::radians(90.0f),(float)(Window::width)/(float)(Window::height),0.01f,10000.0f);
}


void Window::attachCamera(Camera* cam)
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

    activeCamera->rotateFromMouse(dX,dY,dt);

    //movement
    // vec3(fwd/back, up/down, left/right)
    glm::vec3 movement(0.0f);
    if (Controller::getKeyState(window,GLFW_KEY_W)) {
        movement.x+=1.0f;
    }

    if (Controller::getKeyState(window,GLFW_KEY_S)) {
        movement.x-=1.0f;
    }

    if (Controller::getKeyState(window,GLFW_KEY_D)) {
        movement.z+=1.0f;
    }

    if (Controller::getKeyState(window,GLFW_KEY_A)) {
        movement.z-=1.0f;
    }

    if (Controller::getKeyState(window,GLFW_KEY_SPACE)) {
        movement.y+=1.0f;
    }

    if (Controller::getKeyState(window,GLFW_KEY_LEFT_SHIFT)) {
        movement.y-=1.0f;
    }

    glm::vec3 placePos;
    Editing::newBrushes.clear();
    if (Controller::keyPressed(window,GLFW_KEY_P)) {
        Editing::allBrushes.clear();
    }
    if (Controller::getMouseState(window,GLFW_MOUSE_BUTTON_LEFT)) {
        //Place a sphere 1 unit away, with radius 0.1
        placePos = Editing::rayCast( activeCamera->position,activeCamera->getDirection(),Window::mainOctree);
        Editing::placeSphere(placePos,.1);
    }

    if(Controller::mousePressed(window,GLFW_MOUSE_BUTTON_RIGHT)) {
        glm::vec3 pos = activeCamera->getDirection() + activeCamera->position;
        Editing::beginCylinder(pos,0.1);
    }
    if(Controller::mouseReleased(window,GLFW_MOUSE_BUTTON_RIGHT)) {
        glm::vec3 pos = activeCamera->getDirection() + activeCamera->position;
        Editing::endCylinder(pos);
    }

    if (Controller::keyPressed(window,GLFW_KEY_T)) {
        Editing::sphereRing(activeCamera->position,10,1000,1);
    }
    if (Controller::keyPressed(window,GLFW_KEY_F)) {
        Config::wireframe = !Config::wireframe;
    }


    activeCamera->moveFromVec3(movement,dt);
}

void APIENTRY Window::glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}