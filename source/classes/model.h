#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "mesh.h"
#include "shader_program.h"

class Model
{
public:
    explicit Model(const char* path);
    void Draw(Shading::ShaderProgram* shader);

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

private:
    std::vector<Mesh> mMeshes;
    std::vector<Texture> mTexturesLoaded;
    std::string mDirectory;

    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
};
