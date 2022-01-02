#ifndef DRAWING_H
#define DRAWING_H

#include "GL_headers.h"
#include "Window.h"

class Drawing {

    public:
        static GLuint chunkVAO;
        static GLuint lineVAO;
        static GLuint screenVAO;
        static GLuint textVAO;

        static GLuint screenBuffer;
        static GLuint textBuffer;

        static bool init();
        static bool drawFrame();
        static bool drawToScreen();

        static bool initText();
        static bool drawGUI();

        static GLuint loadTexture(char const* Filename);


        static int bufferWidth;
        static int bufferHeight;

        static GLuint frameBuffer;
        static GLuint depthBuffer;
        static GLuint frameBufferTexture;

        struct Character {
            unsigned int TextureID; // ID handle of the glyph texture
            glm::ivec2   Size;      // Size of glyph
            glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
            unsigned int Advance;   // Horizontal offset to advance to next glyph
        };
        static std::map<GLchar, Character> Characters;



    private:

        static bool drawChunks();

        static Shader* chunkShader;
        static Shader* lineShader;
        static Shader* screenShader;
        static Shader* boxShader;
        static Shader* textShader;

        static GLuint crosshairTexture;

        static void drawGUIBox(glm::vec2 position, glm::vec2 size, GLuint texture);
        static void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);

        const static GLfloat screenQuadData[];
        

};


#endif //DRAWING_H