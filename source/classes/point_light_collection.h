#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "point_light.h"
#include "shader.h"

using std::vector;

struct PointLightCollection
{
    PointLightCollection(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic);

    void setLightSettings(glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic);
    void addNewLightAtPosition(glm::vec3 position);

    void updateShader(Shader& shader, glm::mat4 viewMatrix);
    void DrawAll(Shader& shader);

    vector<PointLight> lights;

private:
    glm::vec3 color;

    float ambientLevel;
    float diffuseLevel;
    float specularLevel;

    float constant;
    float linear;
    float quadratic;

    unsigned int VAO, VBO;
};