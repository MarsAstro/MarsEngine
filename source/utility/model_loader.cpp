#include "model_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

Utility::Mesh Utility::ModelLoader::LoadMesh(const char *path)
{
    std::ifstream object;
    object.exceptions(std::ifstream::badbit);

    try
    {
        object.open(path);
    }
    catch (std::ifstream::failure& exception)
    {
        std::cout << "ERROR::ASSET::OBJ_FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    std::vector<Material> materials;
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

        std::stringstream lineStream (currentLine);
        std::string lineWord;
        lineStream >> lineWord;

        if (lineWord == "mtllib")
            materials = ReadMaterialFile(lineStream, path);
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

    for (const Face& face : faces)
    {
        unsigned int baseIndex = vertices.size();

        for (int i = 0; i < face.vertexIndices.size(); ++i)
        {
            unsigned int vertexPosIndex = face.vertexIndices[i];
            unsigned int vertexNormIndex = face.normalIndices[i];
            unsigned int texCoordIndex = face.textureCoordinateIndices[i];

            vertices.push_back({
                vertexPositions[vertexPosIndex - 1],
                vertexNormals[vertexNormIndex - 1],
                textureCoordinates[texCoordIndex - 1],
                GetMaterialIndex(face.materialName, materials)
            });
        }

        for (int i = 1; i < face.vertexIndices.size() - 1; ++i)
        {
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + i);
            indices.push_back(baseIndex + i + 1);
        }
    }

    return { vertices, indices, materials };
}

std::vector<Utility::Material> Utility::ModelLoader::ReadMaterialFile(std::stringstream &objLineStream,
                                                                      const char *objPath)
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
            materials.push_back({ lineWord });
            ++currentMaterial;
        }
        else if (lineWord == "Ns")
            materials[currentMaterial].shininess = ReadFloatFromLine(lineStream);
        else if (lineWord == "Ka")
            materials[currentMaterial].ambient = ReadVec3FromLine(lineStream);
        else if (lineWord == "Kd")
            materials[currentMaterial].diffuse = ReadVec3FromLine(lineStream);
        else if (lineWord == "Ks")
            materials[currentMaterial].specular = ReadVec3FromLine(lineStream);
        else if (lineWord == "Ke")
            materials[currentMaterial].emissive = ReadVec3FromLine(lineStream);
    }

    return materials;
}

Utility::Face Utility::ModelLoader::ReadFaceFromLine(std::stringstream &lineStream, std::string materialName)
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

float Utility::ModelLoader::ReadFloatFromLine(std::stringstream &lineStream)
{
    std::string lineWord;
    lineStream >> lineWord;

    return std::stof(lineWord);
}

glm::vec2 Utility::ModelLoader::ReadVec2FromLine(std::stringstream &lineStream)
{
    glm::vec2 newVector;
    std::string lineWord;

    lineStream >> lineWord;
    newVector.x = std::stof(lineWord);
    lineStream >> lineWord;
    newVector.y = std::stof(lineWord);

    return newVector;
}

glm::vec3 Utility::ModelLoader::ReadVec3FromLine(std::stringstream& lineStream)
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

int Utility::ModelLoader::GetMaterialIndex(const std::string& name, const std::vector<Material> &materials)
{
    int result = -1;

    for (int i = 0; i < materials.size(); ++i)
    {
        if (materials[i].name == name)
        {
            result = i;
            break;
        }
    }

    return result;
}
