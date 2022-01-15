#ifndef DRAWING_H
#define DRAWING_H

#include "GL_headers.h"
#include "Shader.h"
class Drawing {

    public:
        static GLuint chunkVAO;
        static GLuint lineVAO;
        static GLuint screenVAO;

        static GLuint screenBuffer;

        static bool init();
        static bool drawFrame();
        static bool drawToScreen();

        static bool drawGUI();

        static GLuint loadTexture(char const* Filename);


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
        static Shader* boxShader;

        static GLuint crosshairTexture;

        static void drawGUIBox(glm::vec2 position, glm::vec2 size, GLuint texture);

        const static GLfloat screenQuadData[];

};


#endif //DRAWING_H