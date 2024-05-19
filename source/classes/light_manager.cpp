#include "light_manager.h"
#include "../functions.h"

using Shading::Lighting::LightManager;

LightManager::LightManager(unsigned int maxPointLights)
{
    pointLights.resize(maxPointLights);
    CreateOriginCube(0.025f, mVAO, mVBO);
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

void LightManager::AddPointLight(const PointLight &pointLight)
{
    if (mNumPointLights + 1 >= pointLights.size())
        return;

    pointLights[mNumPointLights++] = pointLight;
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
