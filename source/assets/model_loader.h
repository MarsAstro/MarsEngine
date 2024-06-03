#pragma once

#include "../rendering/mesh.h"

namespace Assets
{
    class ModelLoader {
    public:
        static Rendering::Mesh LoadMesh(const char* path);

    private:
        static std::vector<Rendering::Material> ReadMaterialFile(std::stringstream &objLineStream, const char *objPath);
        static unsigned int ReadTextureFromLine(std::stringstream& mtlLineStream, const char* objPath);

        static Rendering::Face ReadFaceFromLine(std::stringstream& lineStream, std::string materialName);
        static float ReadFloatFromLine(std::stringstream& lineStream);
        static glm::vec2 ReadVec2FromLine(std::stringstream& lineStream);
        static glm::vec3 ReadVec3FromLine(std::stringstream& lineStream);

        static int GetMaterialIndex(const std::string& name, const std::vector<Rendering::Material> &materials);
    };
}
