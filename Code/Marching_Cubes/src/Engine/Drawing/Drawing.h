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
        static GLuint lineBuffer;

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

        static void drawGLLine(glm::vec3 pos1, glm::vec3 pos2);
        static void drawAllGLLines();

    private:

        static bool drawChunks();

        static Shader* chunkShader;
        static Shader* lineShader;
        static Shader* screenShader;
        static Shader* boxShader;

        static GLuint crosshairTexture;

        static void drawGUIBox(glm::vec2 position, glm::vec2 size, GLuint texture);

        const static GLfloat screenQuadData[];

        static std::vector<glm::vec4> allGLLines;
        static GLuint rockTexture;
        static GLuint grassTexture;

};


#endif //DRAWING_H