#pragma once

#include "mesh.h"

namespace Geometry
{
    class Model {
    public:
        explicit Model(const char* path, std::vector<Material>* materials);

        void Draw(const Shading::ShaderProgram* shaderProgram) const;

        void SetupInstancing(int amount, const glm::mat4* modelMatrices);
        void DrawInstanced() const;

        glm::vec3 position;
        glm::vec3 scale;

    private:
        static std::vector<Material>    ReadMaterialFile(std::stringstream &objLineStream, const char *objPath);
        static int                      GetMaterialIndex(const std::string& name, const std::vector<Material> &materials);
        static unsigned int             ReadTextureFromLine(std::stringstream& mtlLineStream, const char* objPath);
        static Face                     ReadFaceFromLine(std::stringstream& lineStream, std::string materialName);
        static float                    ReadFloatFromLine(std::stringstream& lineStream);
        static glm::vec2                ReadVec2FromLine(std::stringstream& lineStream);
        static glm::vec3                ReadVec3FromLine(std::stringstream& lineStream);

        Mesh mMesh;
        bool mIsInstancingEnabled;
        int mInstanceAmount;
    };
}
