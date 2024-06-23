#include "model.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>

#include "../assets/import_functions.h"

Geometry::Model::Model(const char *path, std::vector<Material>* materials, unsigned int modelIndex)
    : position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), mInstanceAmount(0), mModelIndex(modelIndex)
{
    std::ifstream object;
    object.exceptions(std::ifstream::badbit);

    try
    {
        object.open(path);
    } catch (std::ifstream::failure &exception)
    {
        std::cout << "ERROR::ASSET::OBJ_FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<Face> faces;
    std::string currentLine;
    std::string currentMaterialName;

    while (object.is_open() && !object.eof())
    {
        std::getline(object, currentLine);
        if (currentLine.empty() || currentLine[0] == '#')
            continue;

        std::stringstream lineStream(currentLine);
        std::string lineWord;
        lineStream >> lineWord;

        if (lineWord == "mtllib")
            materials->append_range(ReadMaterialFile(lineStream, path));
        else if (lineWord == "v")
            vertexPositions.push_back(ReadVec3FromLine(lineStream));
        else if (lineWord == "vn")
            vertexNormals.push_back(ReadVec3FromLine(lineStream));
        else if (lineWord == "vt")
            textureCoordinates.push_back(ReadVec2FromLine(lineStream));
        else if (lineWord == "usemtl")
            lineStream >> currentMaterialName;
        else if (lineWord == "f")
            faces.push_back(ReadFaceFromLine(lineStream, currentMaterialName));
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (const Face &face: faces)
    {
        unsigned int baseIndex = vertices.size();

        for (int i = 0; i < face.vertexIndices.size(); ++i)
        {
            unsigned int vertexPosIndex = face.vertexIndices[i] - 1;
            unsigned int vertexNormIndex = face.normalIndices[i] - 1;
            unsigned int texCoordIndex = face.textureCoordinateIndices[i] - 1;

            vertices.push_back({
                vertexPositions[vertexPosIndex],
                vertexNormals[vertexNormIndex],
                textureCoordinates[texCoordIndex],
                GetMaterialIndex(face.materialName, *materials)
            });
        }

        for (int i = 1; i < face.vertexIndices.size() - 1; ++i)
        {
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + i);
            indices.push_back(baseIndex + i + 1);
        }
    }

    mMesh.SetupMesh(vertices, indices);
}

void Geometry::Model::Draw(const Shading::ShaderProgram* shaderProgram) const
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, scale);
    shaderProgram->SetMat4("model", model);

    glBindVertexArray(mMesh.VAO);
    glDrawElements(GL_TRIANGLES, mMesh.indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Geometry::Model::SetupInstancing(const int amount, const glm::mat4* modelMatrices)
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    glBindVertexArray(mMesh.VAO);
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, nullptr);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(1 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(2 * vec4Size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(3 * vec4Size));

    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);

    mIsInstancingEnabled = true;
    mInstanceAmount = amount;
}

void Geometry::Model::DrawInstanced() const
{
    if (!mIsInstancingEnabled)
    {
        std::cout << "ERROR::MODEL::INSTANCING_NOT_ENABLED" << std::endl;
        return;
    }

    glBindVertexArray(mMesh.VAO);
    glDrawElementsInstanced(GL_TRIANGLES, mMesh.indices.size(), GL_UNSIGNED_INT, nullptr, mInstanceAmount);
    glBindVertexArray(0);
}

std::vector<Geometry::Material> Geometry::Model::ReadMaterialFile(std::stringstream &objLineStream, const char *objPath) const
{
    std::string fileName;
    std::string path = objPath;
    path = path.substr(0, path.find_last_of('/') + 1);

    objLineStream >> fileName;
    path += fileName;

    std::ifstream material;
    material.exceptions(std::ifstream::badbit);

    try
    {
        material.open(path);
    }
    catch (std::ifstream::failure& exception)
    {
        std::cout << "ERROR::ASSET::MTL_FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    std::string currentLine;
    int currentMaterial = -1;
    std::vector<Material> materials;

    while (material.is_open() && !material.eof())
    {
        std::getline(material, currentLine);
        if (currentLine.empty() || currentLine[0] == '#')
            continue;

        std::stringstream lineStream (currentLine);
        std::string lineWord;
        lineStream >> lineWord;

        if (lineWord == "newmtl")
        {
            lineStream >> lineWord;
            materials.push_back({ lineWord, mModelIndex });
            ++currentMaterial;
        }
        else if (lineWord == "Ns")
            materials[currentMaterial].shininess = ReadFloatFromLine(lineStream);
        else if (lineWord == "Ka")
            materials[currentMaterial].ambientColor = ReadVec3FromLine(lineStream);
        else if (lineWord == "Kd")
            materials[currentMaterial].diffuseColor = ReadVec3FromLine(lineStream);
        else if (lineWord == "Ks")
            materials[currentMaterial].specularColor = ReadVec3FromLine(lineStream);
        else if (lineWord == "Ke")
            materials[currentMaterial].emissiveColor = ReadVec3FromLine(lineStream);
        else if (lineWord == "map_Kd")
        {
            materials[currentMaterial].diffuseMap = ReadTextureFromLine(lineStream, objPath, true);
            materials[currentMaterial].hasDiffuseMap = true;
        }
        else if (lineWord == "map_Ks")
        {
            materials[currentMaterial].specularMap = ReadTextureFromLine(lineStream, objPath, false);
            materials[currentMaterial].hasSpecularMap = true;
        }
    }

    return materials;
}

unsigned int Geometry::Model::ReadTextureFromLine(std::stringstream &mtlLineStream, const char *objPath, const bool &isDiffuse)
{
    std::string fileName;
    std::string path = objPath;
    path = path.substr(0, path.find_last_of('/') + 1);

    mtlLineStream >> fileName;
    path += fileName;

    return Assets::LoadTexture(path, isDiffuse);
}

Geometry::Face Geometry::Model::ReadFaceFromLine(std::stringstream &lineStream, std::string materialName)
{
    Face newFace;
    newFace.materialName = std::move(materialName);
    std::string lineWord;

    while (!lineStream.eof())
    {
        lineStream >> lineWord;
        std::stringstream wordStream(lineWord);

        unsigned int values[3];
        for (int i = 0; i < 3 && !wordStream.eof(); ++i)
        {
            std::getline(wordStream, lineWord, '/');
            if (!lineWord.empty())
                values[i] = std::stoi(lineWord);
        }

        newFace.vertexIndices.push_back(values[0]);
        newFace.textureCoordinateIndices.push_back(values[1]);
        newFace.normalIndices.push_back(values[2]);
    }

    return newFace;
}

float Geometry::Model::ReadFloatFromLine(std::stringstream &lineStream)
{
    std::string lineWord;
    lineStream >> lineWord;

    return std::stof(lineWord);
}

glm::vec2 Geometry::Model::ReadVec2FromLine(std::stringstream &lineStream)
{
    glm::vec2 newVector;
    std::string lineWord;

    lineStream >> lineWord;
    newVector.x = std::stof(lineWord);
    lineStream >> lineWord;
    newVector.y = std::stof(lineWord);

    return newVector;
}

glm::vec3 Geometry::Model::ReadVec3FromLine(std::stringstream& lineStream)
{
    glm::vec3 newVector;
    std::string lineWord;

    lineStream >> lineWord;
    newVector.x = std::stof(lineWord);
    lineStream >> lineWord;
    newVector.y = std::stof(lineWord);
    lineStream >> lineWord;
    newVector.z = std::stof(lineWord);

    return newVector;
}

int Geometry::Model::GetMaterialIndex(const std::string& name, const std::vector<Material> &materials) const
{
    int result = -1;

    for (int i = 0; i < materials.size(); ++i)
    {
        if (materials[i].name == name && materials[i].modelIndex == mModelIndex)
        {
            result = i;
            break;
        }
    }

    return result;
}
