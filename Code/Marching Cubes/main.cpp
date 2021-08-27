
// third-party libraries
#include <windows.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "GL_headers.h"

#include "Engine/Window.h"
#include "Engine/Shader.h"
#include "Geometry/Geometry.h"



GLuint gVAO = 0;
GLuint gVBO = 0;
GLuint programId;
GLFWmonitor* monitor = NULL;

const glm::vec2 SCREEN_SIZE(800, 600);

static std::vector<MarchingChunk> loadedChunks;

static void LoadObjects()
{

    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    //GeometryGenerator* G = new CPUMarchingCubesGenerator();
    GeometryGenerator* G = new GPUMarchingCubesGenerator();
    MarchingChunk C(glm::vec3(-5),glm::vec3(100),glm::vec3(.1), G);
    loadedChunks.push_back(C);

    G = new CubeGeometryGenerator();
    MarchingChunk D(glm::vec3(-5),glm::vec3(100),glm::vec3(.1), G);
    //loadedChunks.push_back(D);

}



// draws a single frame

static void Render() {

    //Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw the chunk
    for (auto C : loadedChunks) {
        C.draw(gVAO);

    }

    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(Window::window);

}



// the program starts here

void AppMain() {
    if (!Window::initGL()) {
        printf("Something went wrong, exiting");
        return;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    // make sure OpenGL version 4.2 API is available

    if(!GLEW_VERSION_4_2)

        throw std::runtime_error("OpenGL 4.2 API is not available.");

   // create buffer and fill it with the points of the triangle
    LoadObjects();

    Camera mainCamera;
    Window::attachCamera(mainCamera);

    // load the (test) shader
    Shader shader = Shader::ShaderFromFiles("Shaders/vert.txt","Shaders/frag.txt");
    glUseProgram(shader.getID());

    glm::mat4 VM = glm::lookAt(glm::vec3(2.0,3.0,4.0),glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
    glm::mat4 PM = Window::getProjectionMatrix();
    glUniformMatrix4fv(shader.getUniform("P"),1,GL_FALSE,&PM[0][0]);

    // run while the window is open and focused
    while(true){
        glUseProgram(shader.getID());
    // process pending events
        glfwPollEvents();
        Window::handleInput();

        //set the view matrix accordingly
        VM = Window::activeCamera.getViewMatrix();
        glUniformMatrix4fv(shader.getUniform("V"),1,GL_FALSE,&VM[0][0]);

       // draw one frame
        Render();
    }
   // clean up and exit
   glfwTerminate();

}
int main(int argc, char *argv[]) {

    try {
        AppMain();

    } catch (const std::exception& e){

        std::cout << "ERROR: " << e.what() << std::endl;

        return EXIT_FAILURE;

    }
    return EXIT_SUCCESS;

}
