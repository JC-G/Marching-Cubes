#include "Shader.h"

//load a shader file into a string
std::string Shader::ReadShaderFile(const char* path)
{
    std::string ShaderCode;
    std::ifstream ShaderStream(path,std::ios::in);
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
GLuint Shader::LoadVertexFragment(std::string VertString, std::string FragString)
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
    return ProgramID;
}
Shader Shader::ShaderFromFiles(const char* vertPath, const char* fragPath)
{
    return Shader(
        LoadVertexFragment(
            Shader::ReadShaderFile(vertPath),
            Shader::ReadShaderFile(fragPath)
        )
    );
}

Shader::Shader(GLuint program)
:myProgram(program)
{

}

GLuint Shader::getID()
{
    return myProgram;
}

