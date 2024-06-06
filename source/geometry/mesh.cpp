#include "mesh.h"

#include <../../libraries/glad/include/glad/glad.h>

Geometry::Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    this->mVertices = vertices;
    this->mIndices = indices;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, textureCoordinates)));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, materialIndex)));
}

void Geometry::Mesh::Draw(const Shading::ShaderProgram* shader, const std::vector<Material>& materials) const
{
    for (int i = 0; i < materials.size(); ++i)
    {
        std::string prefix = "materials[" + std::to_string(i) + "].";
        shader->SetVec3(prefix + "ambientColor", materials[i].ambientColor);
        shader->SetVec3(prefix + "diffuseColor", materials[i].diffuseColor);
        shader->SetVec3(prefix + "specularColor", materials[i].specularColor);
        shader->SetVec3(prefix + "emissiveColor", materials[i].emissiveColor);

        shader->SetFloat(prefix + "shininess", materials[i].shininess);

        int textureIndex = i * 2;
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(GL_TEXTURE_2D, materials[i].diffuseMap);
        shader->SetInt(prefix + "diffuseMap", textureIndex);
        shader->SetBool(prefix + "hasDiffuseMap", materials[i].hasDiffuseMap);

        ++textureIndex;
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(GL_TEXTURE_2D, materials[i].specularMap);
        shader->SetInt(prefix + "specularMap", textureIndex);
        shader->SetBool(prefix + "hasSpecularMap", materials[i].hasSpecularMap);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}