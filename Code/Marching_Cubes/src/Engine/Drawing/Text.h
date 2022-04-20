#ifndef TEXT_H
#define TEXT_H

#include "GL_headers.h"
#include "Shader.h"
class Text {
    public:
        static bool initText();
        static void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
    private:
    
        static GLuint textVAO;
        
        static GLuint textBuffer;

        struct Character {
            unsigned int TextureID; // ID handle of the glyph texture
            glm::ivec2   Size;      // Size of glyph
            glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
            unsigned int Advance;   // Horizontal offset to advance to next glyph
        };
        static std::map<GLchar, Character> Characters;

        static Shader* textShader;
        static float scaledHeight;
};

#endif