#include "light_manager.h"

#include <ext/matrix_transform.hpp>
#include <glad/glad.h>

#include "../../constants.h"
#include "../../geometry/geometry_functions.h"

using Shading::Lighting::LightManager;

LightManager::LightManager(unsigned int maxPointLights): directionalLight(), directionalShadow(), mVAO(0), mVBO(0)
{
    pointLights.resize(maxPointLights);
    Geometry::CreateCube(0.025f, mVAO, mVBO);

    glGenTextures(1, &directionalShadow.depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, directionalShadow.depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Constants::SHADOW_WIDTH, Constants::SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glGenFramebuffers(1, &directionalShadow.depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, directionalShadow.depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalShadow.depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightManager::SetDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
    directionalLight = DirectionalLight
    {
        glm::vec4(direction, 1.0f),
        glm::vec4(ambient, 1.0f),
        glm::vec4(diffuse, 1.0f),
        glm::vec4(specular, 1.0f)
    };

    directionalShadow.position = directionalLight.direction * -10.0f;
}

void LightManager::AddPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                                 float constant, float linear, float quadratic)
{
    if (mNumPointLights + 1 >= pointLights.size())
        return;

    PointLight newPointLight
    {
        glm::vec4(position, 1.0f),
        glm::vec4(ambient, 1.0f),
        glm::vec4(diffuse, 1.0f),
        glm::vec4(specular, 1.0f),
        constant,
        linear,
        quadratic
    };

    pointLights[mNumPointLights++] = newPointLight;
}

void LightManager::MovePointLight(unsigned int index, glm::vec3 newPosition)
{
    if (index >= mNumPointLights)
        return;

    pointLights[index].position = glm::vec4(newPosition, 1.0f);
}

void LightManager::DrawPointLightCubes(const ShaderProgram *shaderProgram) const
{
    for (int i = 0; i < mNumPointLights; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = translate(model, glm::vec3(pointLights[i].position));

        glm::vec3 diffuse = glm::vec3(pointLights[i].diffuse);
        glm::vec3 color = (1.0f / diffuse) * diffuse;
        shaderProgram->SetMat4("model", model);
        shaderProgram->SetVec3("objectColor", color);

        glBindVertexArray(mVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

unsigned int LightManager::GetNumberOfPointLights() const
{
    return mNumPointLights;
}

std::vector<Shading::Lighting::PointLight> LightManager::GetViewSpacePointLights(const glm::mat4& viewMatrix) const
{
    std::vector<PointLight> viewSpacePointLights = pointLights;

    for (int i = 0; i < mNumPointLights; ++i)
    {
        glm::vec4 viewSpacePos = viewMatrix * viewSpacePointLights[i].position;
        viewSpacePointLights[i].position = viewSpacePos;
    }

    return viewSpacePointLights;
}

Shading::Lighting::DirectionalLight LightManager::GetViewSpaceDirectionalLight(const glm::mat4& viewMatrix) const
{
    DirectionalLight viewSpaceDirLight = directionalLight;
    viewSpaceDirLight.direction = viewMatrix * viewSpaceDirLight.direction;

    return viewSpaceDirLight;
}

glm::vec3 LightManager::GetDirectionalLightDirection() const
{
    return directionalLight.direction;
}

Shading::Lighting::DirectionalShadow Shading::Lighting::LightManager::GetDirectionalShadow() const {
    return directionalShadow;
}
