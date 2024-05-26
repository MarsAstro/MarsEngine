#pragma once

#include "mesh.h"

namespace Utility
{
    class ModelLoader {
    public:
        static Mesh LoadMesh(const char* path);
    };
}
