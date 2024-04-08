#include "point_light_collection.h"
#include "../functions.h"

PointLightCollection::PointLightCollection(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic)
{
    setLightSettings(color, ambientLevel, diffuseLevel, specularLevel, constant, linear, quadratic);
}

void PointLightCollection::setLightSettings(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic)
{
    this->color = color;

    this->ambientLevel = ambientLevel;
    this->diffuseLevel = diffuseLevel;
    this->specularLevel = specularLevel;

    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;

    createOriginCube(0.025f, VAO, VBO);
}

void PointLightCollection::addNewLightAtPosition(glm::vec3 position)
{
    PointLight newLight = PointLight(position, color, ambientLevel, diffuseLevel, specularLevel, constant, linear, quadratic, VAO);
    lights.push_back(newLight);
}

void PointLightCollection::updateShader(Shader& shader, glm::mat4 viewMatrix)
{
    shader.setInt("numPointLights", lights.size());

    for (unsigned int i = 0; i < lights.size(); i++)
    {
        lights[i].updateShader(shader, viewMatrix, i);
    }
}

void PointLightCollection::DrawAll(Shader& shader)
{
    for (PointLight light : lights)
        light.Draw(shader);
}
