#pragma once

#include <../../libraries/glm/glm.hpp>
#include <vector>

#include "../shading/shader_program.h"

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

    class Mesh {
    public:
        std::vector<Vertex> mVertices;
        std::vector<unsigned int> mIndices;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

        void Draw(const Shading::ShaderProgram* shader, const std::vector<Material>& materials) const;
        void DrawInstanced(const Shading::ShaderProgram* shader, const std::vector<Material>& materials, int amount) const;

        unsigned int VAO, VBO, EBO;

    private:
        static void SetMaterials(const Shading::ShaderProgram *shader, const std::vector<Material> &materials);
    };
}


