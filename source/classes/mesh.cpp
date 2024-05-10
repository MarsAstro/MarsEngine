#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
{
    this->mVertices = vertices;
    this->mIndices = indices;
    this->mTextures = textures;

    SetupMesh();
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoordinates));
}

void Mesh::Draw(Shader* shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < mTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = mTextures[i].type;

        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else
            number = std::to_string(specularNr++);

        shader->SetInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}