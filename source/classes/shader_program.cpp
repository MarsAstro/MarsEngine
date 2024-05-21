#include "shader_program.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glad/glad.h>

using Shading::ShaderProgram;

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    /*

        READ CODE FROM FILES

    */
    std::string vertexCodeString;
    std::string fragmentCodeString;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;

    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);
        std::stringstream vertexShaderStream, fragmentShaderStream;

        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();

        vertexCodeString = vertexShaderStream.str();
        fragmentCodeString = fragmentShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char* vertexShaderCodeCString = vertexCodeString.c_str();
    const char* fragmentShaderCodeCString = fragmentCodeString.c_str();

    /*

        COMPILE CODE

    */
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCodeCString, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCodeCString, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    mID = glCreateProgram();
    glAttachShader(mID, vertex);
    glAttachShader(mID, fragment);
    glLinkProgram(mID);

    glGetProgramiv(mID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(mID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

ShaderProgram::ShaderProgram(const char *vertexPath, const char *geometryPath, const char *fragmentPath)
{
    /*

        READ CODE FROM FILES

    */
    std::string vertexCodeString;
    std::string geometryCodeString;
    std::string fragmentCodeString;
    std::ifstream vertexShaderFile;
    std::ifstream geometryShaderFile;
    std::ifstream fragmentShaderFile;

    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertexShaderFile.open(vertexPath);
        geometryShaderFile.open(geometryPath);
        fragmentShaderFile.open(fragmentPath);
        std::stringstream vertexShaderStream, geometryShaderStream, fragmentShaderStream;

        vertexShaderStream << vertexShaderFile.rdbuf();
        geometryShaderStream << geometryShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        geometryShaderFile.close();
        fragmentShaderFile.close();

        vertexCodeString = vertexShaderStream.str();
        geometryCodeString = geometryShaderStream.str();
        fragmentCodeString = fragmentShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char* vertexShaderCodeCString = vertexCodeString.c_str();
    const char* geometryShaderCodeCString = geometryCodeString.c_str();
    const char* fragmentShaderCodeCString = fragmentCodeString.c_str();

    /*

        COMPILE CODE

    */
    unsigned int vertex, geometry, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCodeCString, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &geometryShaderCodeCString, nullptr);
    glCompileShader(geometry);

    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geometry, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCodeCString, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    mID = glCreateProgram();
    glAttachShader(mID, vertex);
    glAttachShader(mID, geometry);
    glAttachShader(mID, fragment);
    glLinkProgram(mID);

    glGetProgramiv(mID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(mID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(mID);
}

void ShaderProgram::Use() const
{
    glUseProgram(mID);
}

void ShaderProgram::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(mID, name.c_str()), (int)value);
}

void ShaderProgram::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
}

void ShaderProgram::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
}

void ShaderProgram::SetVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(mID, name.c_str()), x, y);
}

void ShaderProgram::SetVec2(const std::string& name, glm::vec2 value) const
{
    glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
}

void ShaderProgram::SetVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(mID, name.c_str()), x, y, z);
}

void ShaderProgram::SetVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
}

void ShaderProgram::SetVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(mID, name.c_str()), x, y, z, w);
}

void ShaderProgram::SetVec4(const std::string& name, glm::vec4 value) const
{
    glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
}

void ShaderProgram::SetMat4(const std::string& name, glm::mat4 matrix) const
{
    unsigned int location = glGetUniformLocation(mID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
