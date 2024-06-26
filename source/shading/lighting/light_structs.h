#pragma once

#include <glm.hpp>

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

    struct DirectionalLight
    {
        glm::vec4 direction;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
    };

    struct DirectionalShadow
    {
        glm::vec3 position;
        unsigned int depthMapFBO;
        unsigned int depthMapTexture;
    };
}