
// third-party libraries
#include <windows.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <sys/param.h>

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
#include "Drawing.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#undef TINYOBJLOADER_IMPLEMENTATION

#include "BulletTest.h"

Octree* O;

static void LoadObjects()
{
    std::string terrainMode = Config::get<std::string>("terrain_mode");
    SDF* usedSDF;
    if (terrainMode == "plane") {
        usedSDF = new PlaneSDF();
    } else if (terrainMode == "terrain") {
        usedSDF = new NoiseTerrain();
    } else if (terrainMode == "sphere") {
        usedSDF = new Sphere(glm::vec3(0.0),1.3);
    } else if (terrainMode == "sin") {
        usedSDF = new SinTerrain();
    }
    GeometryGenerator* G;
    std::string generatorMode = Config::get<std::string>("generator_mode");
    if (generatorMode == "transvoxel") {
        G = new TransvoxelGenerator(usedSDF);
    } else if (generatorMode == "CPU") {
        G = new CPUMarchingCubesGenerator(usedSDF);
    } else if (generatorMode == "GPU") {
        G = new GPUMarchingCubesGenerator(usedSDF);
    }
    if (Config::get<bool>("single_chunk_mode")) {
        //test code to generate a single chunk
        MarchingChunk::loadedChunks.push_back(new MarchingChunk(glm::vec3(1,0,1),glm::vec3(4),glm::vec3(0.25),G,0b010001));

        // loadedChunks.push_back(new MarchingChunk(glm::vec3(-1,-1,1),glm::vec3(4),glm::vec3(0.5),G,0b000000));
    }
    if (Config::get<bool>("load_octree")) {
        O = new Octree(glm::vec3(Config::get<float>("octree_size")),glm::vec3(Config::get<float>("octree_size") * -0.5),0,G);
        Window::mainOctree = O;
        O->update(glm::vec3(0));

    }
    if (Config::get<bool>("show_test_cube")) {
        GeometryGenerator* C = new CubeGeometryGenerator();
        MarchingChunk::loadedChunks.push_back(new MarchingChunk(glm::vec3(0),glm::vec3(0),glm::vec3(0),C,0));
    }
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

    if(!GLEW_VERSION_4_2) {
        throw std::runtime_error("OpenGL 4.2 API is not available.");
    }


    Window::attachCamera(new Camera);

    if (!Drawing::init()) {
        printf("Error initializing framebuffer");
        return;
    }
    
    LoadObjects();

    // Main loop
    while(!glfwWindowShouldClose(Window::window)){
        // process pending events
        glfwPollEvents();
        Window::handleInput();

        //Update the octree
        if (Config::get<bool>("load_octree")) {
            if (Config::get<bool>("update_octree")) {
                O->update(Window::activeCamera->position);
            }
            O->generateAllChunks();
        }

        // draw one frame
        Drawing::drawFrame();
    }
   // clean up and exit
   glfwTerminate();

}
int main(int argc, char *argv[]) {

BulletTest::test();
    char buffer[MAXPATHLEN];
    //Set the working directory - if possible
    if (chdir(WORKING_DIRECTORY) == 0) {
        std::cout << "Working directory found - using " << getcwd(buffer,MAXPATHLEN);
    } else {
        std::cout << "Working directory not found - using " << getcwd(buffer,MAXPATHLEN);
    }
    try {
        AppMain();

    } catch (const std::exception& e){

        std::cout << "ERROR: " << e.what() << std::endl;

        return EXIT_FAILURE;

    }
    return EXIT_SUCCESS;

}
