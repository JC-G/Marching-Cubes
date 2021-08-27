#include "Shader.h"

//load a shader file into a string
std::string Shader::ReadShaderFile(const char* path)
{
    std::string ShaderCode;
    std::ifstream ShaderStream(path,std::ifstream::in);
    if(ShaderStream.is_open())
    {
        std::string line = "";
        while (std::getline(ShaderStream,line))
        {
            ShaderCode += "\n" + line;
        }
        ShaderStream.close();
        return ShaderCode;
    }
    else
    {
        printf("Could not read file");
        return "";
    }
}

//Load a vertex and fragment shader from strings, returning the resulting program
//Also load the uniforms into the map
GLuint Shader::LoadVertexFragment(std::string VertString, std::string FragString, std::map<std::string,GLuint>& shaderUniforms)
{
    GLint Result = GL_FALSE;
    int InfoLogLength;

    //Compile Vertex Shader
    GLuint VertID = glCreateShader(GL_VERTEX_SHADER);
    char const* VertPointer = VertString.c_str();
    glShaderSource(VertID,1,&VertPointer,NULL);
    glCompileShader(VertID);
    glGetShaderiv(VertID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    //Compile Fragment Shader
    GLuint FragID = glCreateShader(GL_FRAGMENT_SHADER);
    char const* FragPointer = FragString.c_str();
    glShaderSource(FragID,1,&FragPointer,NULL);
    glCompileShader(FragID);
    glGetShaderiv(FragID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }



    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID,VertID);
    glAttachShader(ProgramID,FragID);
    glLinkProgram(ProgramID);


    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    SetUniforms(ProgramID,shaderUniforms);

    return ProgramID;
}

Shader Shader::ShaderFromFiles(const char* vertPath, const char* fragPath)
{
    std::map<std::string,GLuint> shaderUniforms;
    GLuint shaderID = LoadVertexFragment(
        Shader::ReadShaderFile(vertPath),
        Shader::ReadShaderFile(fragPath),
        shaderUniforms
    );
    return Shader(shaderID, shaderUniforms);
}

Shader::Shader(GLuint program, std::map<std::string,GLuint> shaderUniforms)
:myProgram(program)
{
    Uniforms = shaderUniforms;
}

GLuint Shader::getID()
{
    return myProgram;
}

GLuint Shader::getUniform(std::string name)
{
    //Uniform list should not be changed at runtime
    return Uniforms[name];
}

GLuint Shader::LoadComputeShader(std::string ShaderText, std::map<std::string, GLuint>& shaderUniforms)
{
    GLuint ComputeID = glCreateShader(GL_COMPUTE_SHADER);

    char const* textPointer = ShaderText.c_str();
    glShaderSource(ComputeID,1,&textPointer,NULL);

    //debug info - compiling
    GLint Result = GL_FALSE;
    int InfoLogLength;
    glGetShaderiv(ComputeID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ComputeID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(ComputeID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID,ComputeID);
    glLinkProgram(ProgramID);

    //debug info - linking
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    SetUniforms(ProgramID, shaderUniforms);

    return ProgramID;
}

void Shader::SetUniforms(GLuint ProgramID, std::map<std::string, GLuint>& shaderUniforms)
{
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length
    glGetProgramiv(ProgramID, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    for (int i = 0; i < count; i++)
    {
        glGetActiveUniform(ProgramID, (GLuint)i, bufSize, &length, &size, &type, name);

        printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
        std::string nameString(name);
        shaderUniforms[nameString] = i;
    }
}

Shader Shader::ComputeShaderFromFile(const char* shaderPath)
{
    std::map<std::string,GLuint> shaderUniforms;
    std::string shaderText = ReadShaderFile("Shaders/Compute/common.txt") + "\n" + ReadShaderFile(shaderPath);
    GLuint shaderID = LoadComputeShader(shaderText,shaderUniforms);
    return Shader(shaderID,shaderUniforms);
}

