#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "shader.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TextureCoordinates;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex>			mVertices;
    std::vector<unsigned int>	mIndices;
    std::vector<Texture>		mTextures;

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
    void Draw(Shader* shader);

private:
    unsigned int mVAO{}, mVBO{}, mEBO{};

    void SetupMesh();
};
