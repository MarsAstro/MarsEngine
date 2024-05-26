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
        std::vector<unsigned int> vertices;
        std::vector<unsigned int> textureCoordinates;
        unsigned int normal;
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


