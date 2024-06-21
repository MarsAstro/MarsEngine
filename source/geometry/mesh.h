#pragma once

#include "geometry_structs.h"
#include <vector>

#include "../shading/shader_program.h"

namespace Geometry
{
    class Mesh {
    public:
        void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO;
    };
}


