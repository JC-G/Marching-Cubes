#include "Drawing.h"

GLuint Drawing::chunkVAO = 0;
GLuint Drawing::lineVAO = 0;
Shader* Drawing::chunkShader;
Shader* Drawing::lineShader;

bool Drawing::init() {
    
    // make and bind the VAO
    glGenVertexArrays(1, &chunkVAO);
    glGenVertexArrays(1, &lineVAO);

    chunkShader = new Shader(Shader::ShaderFromFiles("Shaders/vert.glsl","Shaders/frag.glsl"));
    lineShader = new Shader(Shader::ShaderFromFiles("Shaders/line_vert.glsl","Shaders/line_frag.glsl"));

    glm::mat4 PM = Window::getProjectionMatrix();

    glUseProgram(chunkShader->getID());
    glUniformMatrix4fv(chunkShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

    glUseProgram(lineShader->getID());
    glUniformMatrix4fv(lineShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

    return true;
}

bool Drawing::drawFrame() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawChunks();

    glfwSwapBuffers(Window::window);
    return true;
}

bool Drawing::drawChunks() {

    
    glUseProgram(chunkShader->getID());

    //set the view matrix accordingly
    glm::mat4 VM = Window::activeCamera->getViewMatrix();
    glUniformMatrix4fv(chunkShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
    glUniform3fv(chunkShader->getUniform("cameraPosition"),1,&(Window::activeCamera->position)[0]);

    for (auto C : MarchingChunk::loadedChunks) {
        //for test chunks only, octree is used to store actual chunks
        C->draw(Drawing::chunkVAO);
    }
    if (Config::get<bool>("load_octree")) {
        Window::mainOctree->draw(Drawing::chunkVAO);
    }

    
    if (Config::get<bool>("draw_chunk_boundaries")) {
        glUseProgram(lineShader->getID());
        glUniformMatrix4fv(lineShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
        for (auto C : MarchingChunk::loadedChunks) {
            C->drawBoundary(Drawing::lineVAO);
        }
        if (Config::get<bool>("load_octree")) {
            Window::mainOctree->drawBoundary(Drawing::lineVAO);
        }
    }


    return true;

}

