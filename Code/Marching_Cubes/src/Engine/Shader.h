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
#include "Config.h"

#include "GL_headers.h"
class Shader
{
    public:
        static Shader ShaderFromFiles(const char* vertPath, const char* fragPath);
        GLuint getID();
        GLuint getUniform(std::string name);
        static Shader ComputeShaderFromFile(const char* shaderPath, std::string SDFCode);
        static Shader ComputeShaderFromVector(std::vector<std::string> shaderParts);
        static std::string ReadShaderFile(const char* path);
    private:
        Shader(GLuint program, std::map<std::string,GLuint> shaderUniforms);
        static GLuint LoadVertexFragment(std::string VertString, std::string FragString, std::map<std::string,GLuint>& shaderUniforms);
        GLuint myProgram;
        std::map<std::string,GLuint> Uniforms;

        static void SetUniforms(GLuint programID, std::map<std::string,GLuint>& shaderUniforms);

        //compute shader loading
        static GLuint LoadComputeShader(std::string ShaderText, std::map<std::string,GLuint>& shaderUniforms);

};

#endif // SHADER_H
