#include "shader_manager.h"

#include <gtc/type_ptr.hpp>
#include "glad/glad.h"

using Shading::ShaderManager;
using Shading::ShaderProgram;

ShaderManager::ShaderManager() : lightManager(MAX_POINT_LIGHTS)
{
    /*
     * Create Matrices buffer
     */
    glGenBuffers(1, &mUBOMatrices);

    unsigned int matricesSize = MATRICES_COUNT * sizeof(glm::mat4);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferData(GL_UNIFORM_BUFFER, matricesSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mUBOMatrices);

    /*
     * Create Lights buffer
     */
    glGenBuffers(1, &mUBOPointLights);

    unsigned int pointlightsSize = POINT_LIGHTS_ARRAY_SIZE + sizeof(int);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferData(GL_UNIFORM_BUFFER, pointlightsSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, mUBOPointLights);
}

ShaderProgram* ShaderManager::CreateShaderProgram(const char *vertexPath, const char *fragmentPath)
{
    mShaderProgramList.push_back(std::make_unique<ShaderProgram>(vertexPath, fragmentPath));

    return mShaderProgramList.back().get();
}

ShaderProgram* ShaderManager::CreateShaderProgram(const char* vertexPath, const char* fragmentPath,
    const std::initializer_list<ShaderUniformBlock> uniformBlocks)
{
    mShaderProgramList.push_back(std::make_unique<ShaderProgram>(vertexPath, fragmentPath));
    ShaderProgram* newShader = mShaderProgramList.back().get();

    int returnValue;
    glGetProgramiv(newShader->mID, GL_ACTIVE_UNIFORM_BLOCKS, &returnValue);

    for (ShaderUniformBlock uniformBlock : uniformBlocks)
    {
        int uniformBlockIndex = glGetUniformBlockIndex(newShader->mID, GetUniformBlockLayoutName(uniformBlock));
        glUniformBlockBinding(newShader->mID, uniformBlockIndex, uniformBlock);
    }

    return newShader;
}

ShaderProgram * ShaderManager::CreateShaderProgram(const char *vertexPath,const char *geometryPath, const char *fragmentPath)
{
    mShaderProgramList.push_back(std::make_unique<ShaderProgram>(vertexPath, geometryPath, fragmentPath));

    return mShaderProgramList.back().get();
}

ShaderProgram * ShaderManager::CreateShaderProgram(const char *vertexPath, const char *geometryPath, const char *fragmentPath,
    std::initializer_list<ShaderUniformBlock> uniformBlocks)
{
    mShaderProgramList.push_back(std::make_unique<ShaderProgram>(vertexPath, geometryPath, fragmentPath));
    ShaderProgram* newShader = mShaderProgramList.back().get();

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

void ShaderManager::SetMatrices(const glm::mat4 &view, const glm::mat4 &projection) const
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

void ShaderManager::UpdateLightsBuffer(const glm::mat4 &viewMatrix) const
{
    std::vector<Lighting::PointLight> pointLights = lightManager.GetViewSpacePointLights(viewMatrix);
    int numPointlights = lightManager.GetNumberOfPointLights();

    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, POINT_LIGHTS_ARRAY_SIZE, &pointLights[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, POINT_LIGHTS_ARRAY_SIZE, sizeof(int), static_cast<void*>(&numPointlights));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
