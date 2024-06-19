#pragma once

#include "geometry_structs.h"
#include <vector>

#include "../shading/shader_program.h"

namespace Geometry
{
    class Mesh {
    public:
        std::vector<Vertex> mVertices;
        std::vector<unsigned int> mIndices;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

        void Draw() const;
        void DrawInstanced(int amount) const;

        unsigned int VAO, VBO, EBO;
    };
}


