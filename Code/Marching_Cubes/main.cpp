
// third-party libraries
#include <windows.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "GL_headers.h"

#include "Engine/Window.h"
#include "Engine/Shader.h"
#include "Geometry/Geometry.h"


#include "SDF/Sphere.h"
#include "SDF/SinTerrain.h"
#include "SDF/NoiseTerrain.h"
#include "SDF/TestSDF.h"
#include "SDF/PlaneSDF.h"
#include "Octree.h"
#include "Config.h"



GLuint gVAO = 0;
GLuint gVBO = 0;
GLuint programId;
GLFWmonitor* monitor = NULL;

static std::vector<MarchingChunk*> loadedChunks;
Octree* O;

static void LoadObjects()
{

    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    //GeometryGenerator* G = new TransvoxelGenerator(new Sphere(glm::vec3(0.0),1.3));
    //GeometryGenerator* G = new GPUMarchingCubesGenerator(new NoiseTerrain());
    GeometryGenerator* G = new TransvoxelGenerator(new NoiseTerrain());
    //GeometryGenerator* G = new TransvoxelGenerator(new PlaneSDF());
    if (Config::get<bool>("single_chunk_mode")) {
        //test code to generate a single chunk
        int testEdgeIndex = 1;
        loadedChunks.push_back(new MarchingChunk(glm::vec3(-2,0,4),glm::vec3(4),glm::vec3(0.5),G,32));
    }
    if (Config::get<bool>("load_octree")) {
        O = new Octree(glm::vec3(Config::get<float>("octree_size")),glm::vec3(Config::get<float>("octree_size") * -0.5),0,G);
        O->update(glm::vec3(0));

    }
    if (Config::get<bool>("show_test_cube")) {
        GeometryGenerator* C = new CubeGeometryGenerator();
        loadedChunks.push_back(new MarchingChunk(glm::vec3(0),glm::vec3(0),glm::vec3(0),C,0));
    }
}



// draws a single frame

static void Render() {

    //Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw the chunk
    for (auto C : loadedChunks) {
        C->draw(gVAO);
    }
    if (Config::get<bool>("load_octree")) {
        O->draw(gVAO);
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

    Window::attachCamera(new Camera);

    LoadObjects();

    // load the (test) shader
    Shader shader = Shader::ShaderFromFiles("Shaders/vert.txt","Shaders/frag.txt");
    glUseProgram(shader.getID());

    glm::mat4 VM = glm::lookAt(glm::vec3(2.0,3.0,4.0),glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
    glm::mat4 PM = Window::getProjectionMatrix();
    glUniformMatrix4fv(shader.getUniform("P"),1,GL_FALSE,&PM[0][0]);


    // run while the window is open and focused
    while(true){
    // process pending events
        glfwPollEvents();
        Window::handleInput();

        if (Config::get<bool>("load_octree")) {
            if (Config::get<bool>("update_octree")) {
                O->update(Window::activeCamera->position);
            }
            O->generateAllChunks();
        }

        glUseProgram(shader.getID());

        //set the view matrix accordingly
        VM = Window::activeCamera->getViewMatrix();
        glUniformMatrix4fv(shader.getUniform("V"),1,GL_FALSE,&VM[0][0]);
        glUniform3fv(shader.getUniform("cameraPosition"),1,&(Window::activeCamera->position)[0]);

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
