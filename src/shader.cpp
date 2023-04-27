#include "shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char* geometryPath,
               const char *tcsPath, const char *tesPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tcsCode;
    std::string tesCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    std::ifstream tcsShaderFile;
    std::ifstream tesShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    tcsShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    tesShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if(geometryPath != nullptr)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
        if(tcsPath != nullptr)
        {
            tcsShaderFile.open(tcsPath);
            std::stringstream tcsShaderStream;
            tcsShaderStream << tcsShaderFile.rdbuf();
            tcsShaderFile.close();
            tcsCode = tcsShaderStream.str();
        }
        if(tesPath != nullptr)
        {
            tesShaderFile.open(tesPath);
            std::stringstream tesShaderStream;
            tesShaderStream << tesShaderFile.rdbuf();
            tesShaderFile.close();
            tesCode = tesShaderStream.str();
        }
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << e.what() << '\n';
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    std::cout<<"load vert shader:"<<vertexPath<<std::endl;
    std::cout<<"load frag shader:"<<fragmentPath<<std::endl;

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout<<"vertex shader compile fail "<< infoLog <<std::endl;
    }
    else
    {
        std::cout<<"vertex shader compile success"<<std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout<<"fragment shader compile fail "<< infoLog << std::endl;
    }
    else
    {
        std::cout<<"fragment shader compile success"<<std::endl;
    }

    unsigned int geometry;
    if(geometryPath != nullptr)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);

        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(geometry, 512, NULL, infoLog);
            std::cout<<"geometry shader compile fail "<< infoLog << std::endl;
        }
        else
        {
            std::cout<<"geometry shader compile success"<<std::endl;
        }
    }

    unsigned int tcs;
    if(tcsPath != nullptr)
    {
        const char *tcsShaderCode = tcsCode.c_str();
        tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tcs, 1, &tcsShaderCode, NULL);
        glCompileShader(tcs);

        glGetShaderiv(tcs, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(tcs, 512, NULL, infoLog);
            std::cout<<"tcs shader compile fail "<< infoLog <<std::endl;
        }
        else
        {
            std::cout<<"tcs shader compile success" <<std::endl;
        }
    }

    unsigned int tes;
    if(tesPath != nullptr)
    {
        std::cout<<tesCode<<std::endl;
        const char *tesShaderCode = tesCode.c_str();
        glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tes, 1, &tesShaderCode, NULL);
        glCompileShader(tes);

        glGetShaderiv(tes, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(tes, 512, NULL, infoLog);
            std::cout<<"tes shader compile fail "<< infoLog <<std::endl;
        }
        else
        {
            std::cout<<"tes shader compile success" <<std::endl;
        }
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if(geometryPath != nullptr)
    {
        glAttachShader(ID, geometry);
    }
    if(tcsPath != nullptr)
    {
        glAttachShader(ID, tcs);
    }
    if(tesPath != nullptr)
    {
        glAttachShader(ID, tes);
    }
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout<<"program link fail "<< infoLog <<std::endl;
    }
    else
    {
        std::cout<<"shader program link success"<<std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(geometryPath != nullptr)
    {
        glDeleteShader(geometry);
    }
    if(tcsPath != nullptr)
    {
        glDeleteShader(tcs);
    }
    if(tesPath != nullptr)
    {
        glDeleteShader(tes);
    }
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, glm::vec3 v) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()),1,&v[0]);
}

void Shader::setVec4(const std::string &name, glm::vec4 v) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()),1,&v[0]);
 }
 
 // Activates the Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}