
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
#include "SDF/ScaledTerrain.h"
#include "Octree.h"
#include "Config.h"
#include "Drawing.h"
#include "PhysicsWorld.h"
#include "Timing.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#undef TINYOBJLOADER_IMPLEMENTATION

#include "BulletTest.h"
#include "PhysicsWorld.h"

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
        usedSDF = new Sphere(glm::vec3(100.0),100.0);
    } else if (terrainMode == "sin") {
        usedSDF = new SinTerrain();
    } else if (terrainMode == "scaled") {
        usedSDF = new ScaledTerrain();
    } else if (terrainMode == "test") {
        usedSDF = new TestSDF(Config::get<float>("test_scale"),Config::get<float>("test_iso"));
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
        Timing chunkTimer;
        //test code to generate a single chunk
        usedSDF = new TestSDF(50.0,0.6);
        std::cout << "Beginning chunk timing..." << std::endl;
        chunkTimer.timeDiff();
        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < 1; k++) {
                    MarchingChunk::loadedChunks.push_back(
                        MarchingChunk::createMarchingChunk(
                            glm::vec3(
                                i*Config::get<float>("single_chunk_size")*Config::get<float>("single_chunk_stride"),
                                j*Config::get<float>("single_chunk_size")*Config::get<float>("single_chunk_stride"),
                                k*Config::get<float>("single_chunk_size")*Config::get<float>("single_chunk_stride")
                            ),
                            glm::vec3(Config::get<float>("single_chunk_size")),
                            glm::vec3(Config::get<float>("single_chunk_stride")),
                            G,0b010101
                        )
                    );
                }
            }
        }
        long t = chunkTimer.timeDiff();
        std::cout << "Finished chunk timing: " << t << "ms" << std::endl;
        int total = 0;
        for (MarchingChunk* c : MarchingChunk::loadedChunks) {
            total += c->getGeometrySize();
        }
        std::cout << "Total Triangles: " << total/3 << std::endl;
    }
    if (Config::get<bool>("load_octree")) {
        O = new Octree(glm::vec3(Config::get<float>("octree_size")),glm::vec3(Config::get<float>("octree_size") * -0.5),0,G);
        Window::mainOctree = O;
        O->refresh(glm::vec3(0));

    }
    if (Config::get<bool>("show_test_cube")) {
        GeometryGenerator* C = new CubeGeometryGenerator();
        MarchingChunk::loadedChunks.push_back(MarchingChunk::createMarchingChunk(glm::vec3(0),glm::vec3(0),glm::vec3(0),C,0));
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
    

    PhysicsWorld::init();
    std::cout << "Physics Initialized" << std::endl;

    //load objects after physics world
    LoadObjects();
    Timing physicsTimer;
    physicsTimer.timeDiff();
    if (Config::get<bool>("load_test_brushes")) {
        TestBrushes::generateRandomSpheres();
    }
    // Main loop
    while(!glfwWindowShouldClose(Window::window)){
        // process pending events
        glfwPollEvents();
        Window::handleInput();

        //Update the octree
        if (Config::get<bool>("load_octree")) {
            if (Config::get<bool>("update_octree")) {
                O->refresh(Window::activeCamera->position);
            }
            // O->generateAllChunks();
        }
        PhysicsWorld::step((float)physicsTimer.timeDiff());
        // draw one frame
        Drawing::drawFrame();
    }
   // clean up and exit
   glfwTerminate();

}
int main(int argc, char *argv[]) {

    // BulletTest::test();
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
