#include "shader_manager.h"

#include <gtc/type_ptr.hpp>
#include "glad/glad.h"

using Shading::ShaderManager;
using Shading::ShaderProgram;

ShaderManager::ShaderManager()
{
    mShaderList = std::vector<std::unique_ptr<ShaderProgram>>();

    glGenBuffers(1, &mUBOMatrices);

    unsigned int matricesSize = MATRICES_COUNT * sizeof(glm::mat4);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferData(GL_UNIFORM_BUFFER, matricesSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mUBOMatrices);

    glGenBuffers(1, &mUBOPointLights);

    unsigned int pointlightsSize = POINT_LIGHTS_ARRAY_SIZE + sizeof(int);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferData(GL_UNIFORM_BUFFER, pointlightsSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, mUBOPointLights);
}

ShaderProgram* ShaderManager::CreateShaderProgram(const char *vertexPath, const char *fragmentPath)
{
    mShaderList.push_back(std::make_unique<ShaderProgram>(vertexPath, fragmentPath));

    return mShaderList.back().get();
}

ShaderProgram* ShaderManager::CreateShaderProgram(const char* vertexPath, const char* fragmentPath, const std::initializer_list<ShaderUniformBlock> uniformBlocks)
{
    mShaderList.push_back(std::make_unique<ShaderProgram>(vertexPath, fragmentPath));
    ShaderProgram* newShader = mShaderList.back().get();

    int returnValue;
    glGetProgramiv(newShader->mID, GL_ACTIVE_UNIFORM_BLOCKS, &returnValue);

    for (ShaderUniformBlock uniformBlock : uniformBlocks)
    {
        int uniformBlockIndex = glGetUniformBlockIndex(newShader->mID, GetUniformBlockLayoutName(uniformBlock));
        glUniformBlockBinding(newShader->mID, uniformBlockIndex, uniformBlock);
    }

    return newShader;
}

const char* ShaderManager::GetUniformBlockLayoutName(ShaderUniformBlock uniformBlock)
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

void ShaderManager::SetViewAndProjectionMatrices(const glm::mat4 &view, const glm::mat4 &projection) const
{
    glm::mat4 matrices[] = { view, projection };
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::mat4), matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderManager::SetViewMatrix(glm::mat4 view) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderManager::SetProjectionMatrix(glm::mat4 projection) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderManager::UpdatePointLights(const glm::mat4 &view) const
{
    Lighting::PointLight pointLight { glm::vec4(view * glm::vec4(0.0f, 0.5f, 0.5f, 1.0f)),
        glm::vec4(0.05f),
        glm::vec4(0.5f),
        glm::vec4(1.0f),
        1.0f,
        0.045f,
        0.0075f
    };

    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, SIZEOF_POINT_LIGHT, &pointLight);

    int numPointlights = 1;
    glBufferSubData(GL_UNIFORM_BUFFER, POINT_LIGHTS_ARRAY_SIZE, sizeof(int), static_cast<void*>(&numPointlights));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
