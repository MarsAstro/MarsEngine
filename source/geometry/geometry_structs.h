#pragma once

#include <vector>
#include <../../libraries/glm/glm.hpp>
#include <string>

namespace Geometry
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoordinates;
        int materialIndex;
    };

    struct Face
    {
        std::vector<unsigned int> vertexIndices;
        std::vector<unsigned int> normalIndices;
        std::vector<unsigned int> textureCoordinateIndices;
        std::string materialName;
    };

    struct Material
    {
        std::string name;

        glm::vec3 ambientColor;

        glm::vec3 diffuseColor;
        int diffuseMap;
        bool hasDiffuseMap;

        glm::vec3 specularColor;
        int specularMap;
        bool hasSpecularMap;

        glm::vec3 emissiveColor;

        float shininess;
    };
}
