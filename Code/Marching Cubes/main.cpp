
// third-party libraries
#include <windows.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "GL_headers.h"

#include "Geometry/MarchingChunk.h"
#include "Engine/Window.h"
#include "Engine/Shader.h"

GLuint gVAO = 0;
GLuint gVBO = 0;
GLuint programId;
GLFWmonitor* monitor = NULL;

const glm::vec2 SCREEN_SIZE(800, 600);

static void LoadTriangle()
{

    // make and bind the VAO
   glGenVertexArrays(1, &gVAO);
   glBindVertexArray(gVAO);
   // make and bind the VBO
   glGenBuffers(1, &gVBO);
   glBindBuffer(GL_ARRAY_BUFFER, gVBO);
   // Put the three triangle verticies into the VBO

    GLfloat vertexData[] = {

        //  X     Y     Z     W

         0.0f, 0.8f, 0.0f, 1.0f,

        -0.8f,-0.8f, 0.0f, 1.0f,

         0.8f,-0.8f, 0.0f, 1.0f,

    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    // unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}



// draws a single frame

static void Render() {

    // clear everything
//    glClearColor(0, 0, 1, 1); // blue

    glClear(GL_COLOR_BUFFER_BIT);
  // bind the VAO (the triangle)

    glBindVertexArray(gVAO);
   // draw the VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);
   // unbind the VAO
    glBindVertexArray(0);
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

    LoadTriangle();
    // run while the window is open

    // load the (test) shader
    Shader shader = Shader::ShaderFromFiles("Shaders/vert.txt","Shaders/frag.txt");
    glUseProgram(shader.getID());
    glUniformMatrix4fv(shader.getUniform("V"),1,GL_FALSE,&(glm::mat4(1.0f))[0][0]);
    glUniformMatrix4fv(shader.getUniform("P"),1,GL_FALSE,&(glm::mat4(1.0f))[0][0]);

    while(glfwGetWindowAttrib(Window::window,GLFW_FOCUSED)){
    // process pending events
        glfwPollEvents();
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

        std::cerr << "ERROR: " << e.what() << std::endl;

        return EXIT_FAILURE;

    }
    return EXIT_SUCCESS;

}
