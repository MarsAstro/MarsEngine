#include "shader_manager.h"
#include "glad/glad.h"

ShaderManager::ShaderManager()
{
    shader_list = std::vector<Shader*>();

    glGenBuffers(1, &uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
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

void ShaderManager::SetViewAndProjectionMatrices(glm::mat4 view, glm::mat4 projection) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderManager::SetViewMatrix(glm::mat4 view) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderManager::SetProjectionMatrix(glm::mat4 projection) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
