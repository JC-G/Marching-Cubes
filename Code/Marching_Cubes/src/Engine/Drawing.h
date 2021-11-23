#ifndef DRAWING_H
#define DRAWING_H

#include "GL_headers.h"
#include "Window.h"

class Drawing {

    public:
        static GLuint chunkVAO;
        static GLuint lineVAO;

        static bool init();
        static bool drawFrame();

    private:

        static bool drawChunks();

        static Shader* chunkShader;
        static Shader* lineShader;


};


#endif //DRAWING_H