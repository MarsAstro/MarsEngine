#pragma once

#include <../../libraries/glm/glm.hpp>
#include <vector>

#include "../classes/shader_program.h"

namespace Rendering
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

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Material> materials;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Material>& materials);
        void Draw(const Shading::ShaderProgram* shader) const;

    private:
        unsigned int VAO, VBO, EBO;
    };
}


