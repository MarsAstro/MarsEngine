#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"

class PointLight
{
public:
    PointLight(glm::vec3 position, glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic);
    PointLight(glm::vec3 position, glm::vec3 color, float ambientLevel, float diffuseLevel, float specularLevel, float constant, float linear, float quadratic, unsigned int VAO);

    void UpdateShader(Shading::ShaderProgram* shader, glm::mat4 viewMatrix, int index = -1) const;
    void Draw(Shading::ShaderProgram* shader) const;

    glm::vec3 position;
    glm::vec3 color;

private:
    glm::vec3 mAmbient;
    glm::vec3 mDiffuse;
    glm::vec3 mSpecular;

    float mConstant;
    float mLinear;
    float mQuadratic;

    unsigned int mVAO, mVBO;
};