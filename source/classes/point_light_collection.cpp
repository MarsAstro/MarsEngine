#include "point_light_collection.h"
#include "../functions.h"

PointLightCollection::PointLightCollection(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic)
{
    SetLightSettings(color, ambientLevel, diffuseLevel, specularLevel, constant, linear, quadratic);
}

void PointLightCollection::SetLightSettings(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic)
{
    this->mColor = color;

    this->mAmbientLevel = ambientLevel;
    this->mDiffuseLevel = diffuseLevel;
    this->mSpecularLevel = specularLevel;

    this->mConstant = constant;
    this->mLinear = linear;
    this->mQuadratic = quadratic;

    CreateOriginCube(0.025f, mVAO, mVBO);
}

void PointLightCollection::AddLightAtPosition(glm::vec3 position)
{
    PointLight newLight = PointLight(position, mColor, mAmbientLevel, mDiffuseLevel, mSpecularLevel, mConstant, mLinear, mQuadratic, mVAO);
    lights.push_back(newLight);
}

void PointLightCollection::UpdateShader(Shading::ShaderProgram* shader, const glm::mat4 &viewMatrix) const
{
    shader->SetInt("numPointLights", lights.size());

    for (unsigned int i = 0; i < lights.size(); i++)
    {
        lights[i].UpdateShader(shader, viewMatrix, i);
    }
}

void PointLightCollection::DrawAll(Shading::ShaderProgram* shader) const
{
    for (PointLight light : lights)
        light.Draw(shader);
}
