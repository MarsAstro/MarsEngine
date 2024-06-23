#include "resource_manager.h"

#include <../libraries/glm/gtc/type_ptr.hpp>
#include "../libraries/glad/include/glad/glad.h"

using Shading::ShaderProgram;

ResourceManager::ResourceManager() : lightManager(MAX_POINT_LIGHTS)
{
    /*
     * Create Matrices buffer
     */
    glGenBuffers(1, &mUBOMatrices);

    unsigned int matricesSize = MATRICES_COUNT * sizeof(glm::mat4);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferData(GL_UNIFORM_BUFFER, matricesSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, Matrices, mUBOMatrices);

    /*
     * Create PointLights buffer
     */
    glGenBuffers(1, &mUBOPointLights);

    unsigned int pointlightsSize = MAX_POINT_LIGHTS * sizeof(Shading::Lighting::PointLight) + sizeof(int);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferData(GL_UNIFORM_BUFFER, pointlightsSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, PointLights, mUBOPointLights);
}

ShaderProgram* ResourceManager::CreateShaderProgram(const char *vertexPath, const char *fragmentPath)
{
    mShaderProgramList.push_back(std::make_unique<ShaderProgram>(vertexPath, fragmentPath));

    return mShaderProgramList.back().get();
}

ShaderProgram* ResourceManager::CreateShaderProgram(const char* vertexPath, const char* fragmentPath,
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

ShaderProgram * ResourceManager::CreateShaderProgram(const char *vertexPath,const char *geometryPath, const char *fragmentPath)
{
    mShaderProgramList.push_back(std::make_unique<ShaderProgram>(vertexPath, geometryPath, fragmentPath));

    return mShaderProgramList.back().get();
}

ShaderProgram * ResourceManager::CreateShaderProgram(const char *vertexPath, const char *geometryPath, const char *fragmentPath,
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

Geometry::Model ResourceManager::LoadModel(const char *modelPath)
{
    Geometry::Model newModel = Geometry::Model(modelPath, &mMaterials);

    return newModel;
}

const char* ResourceManager::GetUniformBlockLayoutName(const ShaderUniformBlock uniformBlock)
{
    switch (uniformBlock)
    {
        case Matrices:          return "Matrices";
        case PointLights:       return "PointLights";
        default:                return "";
    }
}

void ResourceManager::SetMatrices(const glm::mat4& view, const glm::mat4& projection) const
{
    glm::mat4 matrices[] = { view, projection };
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::mat4), matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ResourceManager::SetViewMatrix(glm::mat4 view) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ResourceManager::ApplyMaterials(const ShaderProgram* shader) const
{
    shader->Use();

    for (int i = 0; i < mMaterials.size(); ++i)
    {
        std::string prefix = "materials[" + std::to_string(i) + "].";
        shader->SetVec3(prefix + "ambientColor", mMaterials[i].ambientColor);
        shader->SetVec3(prefix + "diffuseColor", mMaterials[i].diffuseColor);
        shader->SetVec3(prefix + "specularColor", mMaterials[i].specularColor);
        shader->SetVec3(prefix + "emissiveColor", mMaterials[i].emissiveColor);

        shader->SetFloat(prefix + "shininess", mMaterials[i].shininess);

        int textureIndex = i * 2;
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(GL_TEXTURE_2D, mMaterials[i].diffuseMap);
        shader->SetInt(prefix + "diffuseMap", textureIndex);
        shader->SetBool(prefix + "hasDiffuseMap", mMaterials[i].hasDiffuseMap);

        ++textureIndex;
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(GL_TEXTURE_2D, mMaterials[i].specularMap);
        shader->SetInt(prefix + "specularMap", textureIndex);
        shader->SetBool(prefix + "hasSpecularMap", mMaterials[i].hasSpecularMap);
    }
}

void ResourceManager::UpdateDirectionalLight(const Shading::ShaderProgram *shader, const glm::mat4& viewMatrix) const
{
    shader->Use();

    Shading::Lighting::DirectionalLight dirLight = lightManager.GetViewSpaceDirectionalLight(viewMatrix);
    std::string prefix = "directionalLight.";
    shader->SetVec4(prefix + "direction",   dirLight.direction);
    shader->SetVec4(prefix + "ambient",     dirLight.ambient);
    shader->SetVec4(prefix + "diffuse",     dirLight.diffuse);
    shader->SetVec4(prefix + "specular",    dirLight.specular);
}

int ResourceManager::GetTextureCount() const
{
    return mMaterials.size() * 2;
}

void ResourceManager::UpdatePointLightsBuffer(const glm::mat4 &viewMatrix) const
{
    std::vector<Shading::Lighting::PointLight> pointLights = lightManager.GetViewSpacePointLights(viewMatrix);
    int numPointlights = lightManager.GetNumberOfPointLights();

    unsigned int pointLightsArraySize = MAX_POINT_LIGHTS * sizeof(Shading::Lighting::PointLight);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBOPointLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, pointLightsArraySize, &pointLights[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, pointLightsArraySize, sizeof(int), static_cast<void*>(&numPointlights));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}