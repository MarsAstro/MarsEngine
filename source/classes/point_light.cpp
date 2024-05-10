#include <string>

#include <glad/glad.h>

#include "point_light.h"
#include "../functions.h"

using std::string;

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel,
                       float constant, float linear, float quadratic)
{
    CreateOriginCube(0.025f, mVAO, mVBO);
    PointLight(position, color, ambientLevel, diffuseLevel, specularLevel, constant, linear, quadratic, mVAO);
}

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel,
                       float constant, float linear, float quadratic, unsigned int VAO)
{
    this->position = position;
    this->color = color;

    mAmbient = color * glm::vec3(ambientLevel);
    mDiffuse = color * glm::vec3(diffuseLevel);
    mSpecular = color * glm::vec3(specularLevel);

    this->mConstant = constant;
    this->mLinear = linear;
    this->mQuadratic = quadratic;

    this->mVAO = VAO;

    mVBO = VAO;
}

void PointLight::UpdateShader(Shader* shader, glm::mat4 viewMatrix, int index) const
{
    string prefix = "pointLight.";

    if (index >= 0)
        prefix = "pointLights[" + std::to_string(index) + "].";

    glm::vec3 viewSpacePosition = glm::vec3(viewMatrix * glm::vec4(position, 1.0f));

    shader->SetVec3(prefix + "position", viewSpacePosition);

    shader->SetVec3(prefix + "ambient", mAmbient);
    shader->SetVec3(prefix + "diffuse", mDiffuse);
    shader->SetVec3(prefix + "specular", mSpecular);

    shader->SetFloat(prefix + "constant", mConstant);
    shader->SetFloat(prefix + "linear", mLinear);
    shader->SetFloat(prefix + "quadratic", mQuadratic);
}

void PointLight::Draw(Shader* shader) const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader->SetMat4("model", model);
    shader->SetVec3("objectColor", color);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
