#include "mesh.h"

#include <glad/glad.h>

Utility::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Material>& materials)
{
    this->vertices = vertices;
    this->indices = indices;
    this->materials = materials;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, textureCoordinates)));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, materialIndex)));
}

void Utility::Mesh::Draw(const Shading::ShaderProgram *shader) const
{
    for (int i = 0; i < materials.size(); ++i)
    {
        std::string prefix = "materials[" + std::to_string(i) + "].";
        shader->SetVec3(prefix + "ambient", materials[i].ambient);
        shader->SetVec3(prefix + "diffuse", materials[i].diffuse);
        shader->SetVec3(prefix + "specular", materials[i].specular);
        shader->SetVec3(prefix + "emissive", materials[i].emissive);
        shader->SetFloat(prefix + "shininess", materials[i].shininess);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
