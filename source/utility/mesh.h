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
    };

    struct Face
    {
        std::vector<unsigned int> vertexIndices;
        std::vector<unsigned int> normalIndices;
        std::vector<unsigned int> textureCoordinateIndices;
    };

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        void Draw(const Shading::ShaderProgram* shader) const;

    private:
        unsigned int VAO, VBO, EBO;
    };
}


