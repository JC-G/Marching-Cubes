#include "Drawing.h"

GLuint Drawing::chunkVAO = 0;
GLuint Drawing::lineVAO = 0;
GLuint Drawing::screenVAO = 0;
GLuint Drawing::screenBuffer = 0;
Shader* Drawing::chunkShader;
Shader* Drawing::lineShader;
Shader* Drawing::screenShader;
int Drawing::bufferWidth;
int Drawing::bufferHeight;
GLuint Drawing::frameBuffer = 0;
GLuint Drawing::depthBuffer = 0;
GLuint Drawing::frameBufferTexture = 0;


bool Drawing::init() {
    
    // make and bind the VAO
    glGenVertexArrays(1, &chunkVAO);
    glGenVertexArrays(1, &lineVAO);
    glGenVertexArrays(1, &screenVAO);

    chunkShader = new Shader(Shader::ShaderFromFiles("Shaders/vert.glsl","Shaders/frag.glsl"));
    lineShader = new Shader(Shader::ShaderFromFiles("Shaders/line_vert.glsl","Shaders/line_frag.glsl"));
    screenShader = new Shader(Shader::ShaderFromFiles("Shaders/screen_vert.glsl","Shaders/screen_frag.glsl"));


    //set uniforms that never change
    glm::mat4 PM = Window::getProjectionMatrix();

    glUseProgram(chunkShader->getID());
    glUniformMatrix4fv(chunkShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

    glUseProgram(lineShader->getID());
    glUniformMatrix4fv(lineShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

    //create framebuffer
    glfwGetFramebufferSize(Window::window,&bufferWidth,&bufferHeight);
    glGenFramebuffers(1,&frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);

    //create blank texture
    glGenTextures(1,&frameBufferTexture);
    glBindTexture(GL_TEXTURE_2D,frameBufferTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,bufferWidth,bufferHeight,0,GL_RGB,GL_UNSIGNED_BYTE,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //create and attach depth buffer
    glGenRenderbuffers(1,&depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,bufferWidth,bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthBuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,frameBufferTexture,0);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //glBindVertexArray(screenVAO);

    static const GLfloat screenQuadData[] = { 
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

    glGenBuffers(1,&screenBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,screenBuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(screenQuadData),screenQuadData,GL_STATIC_DRAW);

    return true;
}

bool Drawing::drawFrame() {

    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
    glViewport(0,0,bufferWidth,bufferHeight);
    glClearColor(0,148./255,1,1);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawChunks();

    drawToScreen();

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

bool Drawing::drawToScreen() {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,bufferWidth,bufferHeight);
    
    glBindVertexArray(screenVAO);

    glClearColor(1,148./255,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(screenShader->getID());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,frameBufferTexture);
    glUniform1i(screenShader->getUniform("renderedTexture"),0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,screenBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDrawArrays(GL_TRIANGLES,0,6);

    glfwSwapBuffers(Window::window);
    return true;
}

