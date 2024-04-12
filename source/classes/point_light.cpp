#include <string>

#include <glad/glad.h>

#include "point_light.h"
#include "../functions.h"

using std::string;

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel,
                       float constant, float linear, float quadratic)
{
    CreateOriginCube(0.025f, VAO, VBO);
    PointLight(position, color, ambientLevel, diffuseLevel, specularLevel, constant, linear, quadratic, VAO);
}

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel,
                       float constant, float linear, float quadratic, unsigned int VAO)
{
    this->position = position;
    this->color = color;

    ambient = color * glm::vec3(ambientLevel);
    diffuse = color * glm::vec3(diffuseLevel);
    specular = color * glm::vec3(specularLevel);

    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;

    this->VAO = VAO;

    VBO = VAO;
}

void PointLight::UpdateShader(Shader* shader, glm::mat4 viewMatrix, int index) const
{
    string prefix = "pointLight.";

    if (index >= 0)
        prefix = "pointLights[" + std::to_string(index) + "].";

    glm::vec3 viewSpacePosition = glm::vec3(viewMatrix * glm::vec4(position, 1.0f));

    shader->SetVec3(prefix + "position", viewSpacePosition);

    shader->SetVec3(prefix + "ambient", ambient);
    shader->SetVec3(prefix + "diffuse", diffuse);
    shader->SetVec3(prefix + "specular", specular);

    shader->SetFloat(prefix + "constant", constant);
    shader->SetFloat(prefix + "linear", linear);
    shader->SetFloat(prefix + "quadratic", quadratic);
}

void PointLight::Draw(Shader* shader) const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader->SetMat4("model", model);
    shader->SetVec3("objectColor", color);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
