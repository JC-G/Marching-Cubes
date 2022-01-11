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
        static bool initOBJ();

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

        static void drawPreviewSphere(glm::vec3 radius, glm::vec3 position);
        static void drawPreviewCylinder(float radius, glm::vec3 pos1, glm::vec3 pos2);
        static void drawPreviewLine(glm::vec3 pos1, glm::vec3 pos2);

    private:

        static bool drawChunks();

        static Shader* chunkShader;
        static Shader* lineShader;
        static Shader* screenShader;
        static Shader* boxShader;
        static Shader* textShader;
        static Shader* spherePreviewShader;

        static GLuint crosshairTexture;

        static void drawGUIBox(glm::vec2 position, glm::vec2 size, GLuint texture);
        static void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);

        const static GLfloat screenQuadData[];
        
	    static std::vector<glm::vec4> sphereVertexData;
	    static std::vector<glm::vec4> sphereNormalData;
        static std::vector<glm::vec4> cylinderVertexData;
        
        static GLuint previewVertexBuffer;
        static GLuint previewNormalBuffer;

        static bool parseOBJ(std::string path, std::vector<glm::vec4>& vertices);

};


#endif //DRAWING_H