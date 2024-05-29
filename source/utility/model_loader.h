#pragma once

#include "mesh.h"

namespace Utility
{
    class ModelLoader {
    public:
        static Mesh LoadMesh(const char* path);

    private:
        static std::vector<Material> ReadMaterialFile(std::stringstream &objLineStream, const char *objPath);

        static Face ReadFaceFromLine(std::stringstream& lineStream, std::string materialName);
        static float ReadFloatFromLine(std::stringstream& lineStream);
        static glm::vec2 ReadVec2FromLine(std::stringstream& lineStream);
        static glm::vec3 ReadVec3FromLine(std::stringstream& lineStream);

        static int GetMaterialIndex(const std::string& name, const std::vector<Material> &materials);
    };
}
