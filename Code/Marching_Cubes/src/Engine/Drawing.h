#ifndef DRAWING_H
#define DRAWING_H

#include "GL_headers.h"
#include "Window.h"

class Drawing {

    public:
        static GLuint chunkVAO;
        static GLuint lineVAO;
        static GLuint screenVAO;

        static GLuint screenBuffer;

        static bool init();
        static bool drawFrame();
        static bool drawToScreen();



        static int bufferWidth;
        static int bufferHeight;

        static GLuint frameBuffer;
        static GLuint depthBuffer;
        static GLuint frameBufferTexture;

    private:

        static bool drawChunks();

        static Shader* chunkShader;
        static Shader* lineShader;
        static Shader* screenShader;

};


#endif //DRAWING_H