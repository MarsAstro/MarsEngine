#include "shader_manager.h"
#include "glad/glad.h"

ShaderManager::ShaderManager()
{
    mShaderList = std::vector<std::unique_ptr<Shader>>();

    glGenBuffers(1, &mUBOMatrices);

    unsigned int matricesSize = mMatricesCount * sizeof(glm::mat4);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferData(GL_UNIFORM_BUFFER, matricesSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mUBOMatrices);

    glGenBuffers(1, &mUBOPointLights);

    unsigned int pointLightsSize = mMaxPointLights * mSizeOfPointLight + sizeof(int);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferData(GL_UNIFORM_BUFFER, pointLightsSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, mUBOPointLights);

    float constant = 1.0f;
    float linear = 0.045f;
    float quadratic = 0.0075f;
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(glm::vec3(0.0f, 0.5f, 0.5f)));
    glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(glm::vec3(0.05f)));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(glm::vec3(0.5f)));
    glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(glm::vec3(1.0f)));

    unsigned int vecOffsets = 3 * sizeof(glm::vec4) + sizeof(glm::vec3);
    glBufferSubData(GL_UNIFORM_BUFFER, vecOffsets + 0 * sizeof(float), sizeof(float), static_cast<void*>(&constant));
    glBufferSubData(GL_UNIFORM_BUFFER, vecOffsets + 1 * sizeof(float), sizeof(float), static_cast<void*>(&linear));
    glBufferSubData(GL_UNIFORM_BUFFER, vecOffsets + 2 * sizeof(float), sizeof(float), static_cast<void*>(&quadratic));

    int numPointlights = 1;
    unsigned int numLightsOffset = pointLightsSize - sizeof(int);
    glBufferSubData(GL_UNIFORM_BUFFER, numLightsOffset, sizeof(int), static_cast<void*>(&numPointlights));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Shader* ShaderManager::CreateShader(const char *vertexPath, const char *fragmentPath)
{
    mShaderList.push_back(std::make_unique<Shader>(vertexPath, fragmentPath));

    return mShaderList.back().get();
}

Shader* ShaderManager::CreateShader(const char* vertexPath, const char* fragmentPath, const std::initializer_list<ShaderUniformBlock> uniformBlocks)
{
    mShaderList.push_back(std::make_unique<Shader>(vertexPath, fragmentPath));
    Shader* newShader = mShaderList.back().get();

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

void ShaderManager::SetViewAndProjectionMatrices(glm::mat4 view, glm::mat4 projection) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
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
