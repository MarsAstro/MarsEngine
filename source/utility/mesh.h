#pragma once

#include <glm.hpp>
#include <vector>

#include "../classes/shader_program.h"

namespace Utility
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
        float shininess;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 emissive;
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


