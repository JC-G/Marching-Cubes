#ifndef SHADER_H
#define SHADER_H

/**
Shader handles generic shader stuff
*/

#include <iostream>
#include <fstream>
#include <vector>

#include "GL_headers.h"
class Shader
{
    public:
        static Shader ShaderFromFiles(const char* vertPath, const char* fragPath);
        GLuint getID();
    private:
        Shader(GLuint program);
        static std::string ReadShaderFile(const char* path);
        static GLuint LoadVertexFragment(std::string VertString, std::string FragString);

        GLuint myProgram;

};

#endif // SHADER_H
