#include "shader_manager.h"
#include "glad/glad.h"

ShaderManager::ShaderManager()
{
    shader_list = std::vector<Shader*>();
}

ShaderManager::~ShaderManager()
{
    for (Shader* shader : shader_list)
    {
        delete shader;
    }
}

Shader* ShaderManager::CreateShader(const char *vertexPath, const char *fragmentPath)
{
    Shader* newShader = new Shader(vertexPath, fragmentPath);
    shader_list.emplace_back(newShader);

    return shader_list.back();
}

Shader* ShaderManager::CreateShader(const char* vertexPath, const char* fragmentPath, const std::initializer_list<ShaderUniformBlock> uniformBlocks)
{
    Shader* newShader = new Shader(vertexPath, fragmentPath);
    shader_list.emplace_back(newShader);

    for (ShaderUniformBlock uniformBlock : uniformBlocks)
    {
        unsigned int uniformBlockIndex = glGetUniformBlockIndex(newShader->ID, GetUniformBlockLayoutName(uniformBlock));
        glUniformBlockBinding(newShader->ID, uniformBlockIndex, uniformBlock);
    }

    return shader_list.back();
}

const char * ShaderManager::GetUniformBlockLayoutName(ShaderUniformBlock uniformBlock)
{
    switch (uniformBlock)
    {
        case Matrices:
            return "Matrices";
        case PointLights:
            return "PointLights";
        default:
            return "";
    }
}