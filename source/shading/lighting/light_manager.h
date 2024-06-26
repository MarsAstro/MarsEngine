#pragma once

#include <vector>
#include <glm.hpp>

#include "../shader_program.h"
#include "light_structs.h"

namespace Shading::Lighting
{
    class LightManager
    {
    public:
        explicit LightManager(unsigned int maxPointLights);

        void SetDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
        void AddPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
        void MovePointLight(unsigned int index, glm::vec3 newPosition);
        void DrawPointLightCubes(const ShaderProgram* shaderProgram) const;

        unsigned int GetNumberOfPointLights() const;
        std::vector<PointLight> GetViewSpacePointLights(const glm::mat4& viewMatrix) const;
        DirectionalLight GetViewSpaceDirectionalLight(const glm::mat4& viewMatrix) const;
        DirectionalShadow GetDirectionalShadow() const;
        glm::vec3 GetDirectionalLightDirection() const;

    private:
        std::vector<PointLight> pointLights;
        DirectionalLight directionalLight;
        DirectionalShadow directionalShadow;

        unsigned int mNumPointLights = 0;
        unsigned int mVAO, mVBO;
    };
}
