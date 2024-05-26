#include "model_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Utility::Mesh Utility::ModelLoader::LoadMesh(const char *path)
{
    std::ifstream obj;
    obj.exceptions(std::ifstream::badbit);

    try
    {
        obj.open(path);
    }
    catch (std::ifstream::failure& exception)
    {
        std::cout << "ERROR::ASSET::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<Face> faces;
    std::string currentLine;

    while (obj.is_open() && !obj.eof())
    {
        std::getline(obj, currentLine);
        if (currentLine.empty() || currentLine[0] == '#')
            continue;

        std::stringstream lineStream (currentLine);
        std::string lineWord;
        lineStream >> lineWord;

        if (lineWord == "v")
            vertexPositions.push_back(ReadVec3FromLine(lineStream));

        if (lineWord == "vn")
            vertexNormals.push_back(ReadVec3FromLine(lineStream));

        if (lineWord == "vt")
            textureCoordinates.push_back(ReadVec2FromLine(lineStream));

        if (lineWord == "f")
            faces.push_back(ReadFaceFromLine(lineStream));
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (const Face& face : faces)
    {
        unsigned int baseIndex = vertices.size();
        glm::vec3 normal = vertexNormals[face.normal - 1];

        for (int i = 0; i < face.vertices.size(); ++i)
        {
            unsigned int vertexPosIndex = face.vertices[i];
            unsigned int texCoordIndex = face.textureCoordinates[i];

            vertices.push_back({ vertexPositions[vertexPosIndex - 1], normal, textureCoordinates[texCoordIndex - 1]});
        }

        for (int i = 1; i < face.vertices.size() - 1; ++i)
        {
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + i);
            indices.push_back(baseIndex + i + 1);
        }
    }

    return { vertices, indices };
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

Utility::Face Utility::ModelLoader::ReadFaceFromLine(std::stringstream &lineStream)
{
    Face newFace;
    std::string lineWord;
    newFace.normal = 0;

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

        newFace.vertices.push_back(values[0]);
        newFace.textureCoordinates.push_back(values[1]);
        newFace.normal = values[2];
    }

    return newFace;
}
