#pragma once

#include <vector>
#include <glm.hpp>

#include "shader_program.h"

namespace Shading::Lighting
{
    struct PointLight
    {
        glm::vec4 position;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;

        float constant;
        float linear;
        float quadratic;
        float PADDING;
    };

    class LightManager
    {
    public:
        explicit LightManager(unsigned int maxPointLights);

        void AddPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
        void AddPointLight(const PointLight &pointLight);
        void DrawPointLightCubes(ShaderProgram* shaderProgram) const;

        unsigned int GetNumberOfPointLights() const;
        std::vector<PointLight> GetViewSpacePointLights(const glm::mat4& viewMatrix) const;

    private:
        std::vector<PointLight> pointLights;

        unsigned int mNumPointLights = 0;
        unsigned int mVAO, mVBO;
    };
}
