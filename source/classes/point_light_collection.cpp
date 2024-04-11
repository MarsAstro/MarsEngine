#include "point_light_collection.h"
#include "../functions.h"

PointLightCollection::PointLightCollection(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic)
{
    SetLightSettings(color, ambientLevel, diffuseLevel, specularLevel, constant, linear, quadratic);
}

void PointLightCollection::SetLightSettings(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic)
{
    this->color = color;

    this->ambientLevel = ambientLevel;
    this->diffuseLevel = diffuseLevel;
    this->specularLevel = specularLevel;

    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;

    CreateOriginCube(0.025f, VAO, VBO);
}

void PointLightCollection::AddLightAtPosition(glm::vec3 position)
{
    PointLight newLight = PointLight(position, color, ambientLevel, diffuseLevel, specularLevel, constant, linear, quadratic, VAO);
    lights.push_back(newLight);
}

void PointLightCollection::UpdateShader(Shader& shader, glm::mat4 viewMatrix)
{
    shader.SetInt("numPointLights", lights.size());

    for (unsigned int i = 0; i < lights.size(); i++)
    {
        lights[i].UpdateShader(shader, viewMatrix, i);
    }
}

void PointLightCollection::DrawAll(Shader& shader)
{
    for (PointLight light : lights)
        light.Draw(shader);
}
