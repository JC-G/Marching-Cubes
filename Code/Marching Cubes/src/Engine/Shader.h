#ifndef SHADER_H
#define SHADER_H

/**
Shader handles generic shader stuff
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <vector>

#include "GL_headers.h"
class Shader
{
    public:
        static Shader ShaderFromFiles(const char* vertPath, const char* fragPath);
        GLuint getID();
        GLuint getUniform(std::string name);
    private:
        Shader(GLuint program, std::map<std::string,GLuint> shaderUniforms);
        static std::string ReadShaderFile(const char* path);
        static GLuint LoadVertexFragment(std::string VertString, std::string FragString, std::map<std::string,GLuint>& shaderUniforms);
        GLuint myProgram;
        std::map<std::string,GLuint> Uniforms;

};

#endif // SHADER_H
