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

        for (int i = 0; i < face.vertexIndices.size(); ++i)
        {
            unsigned int vertexPosIndex = face.vertexIndices[i];
            unsigned int vertexNormIndex = face.normalIndices[i];
            unsigned int texCoordIndex = face.textureCoordinateIndices[i];

            vertices.push_back({ vertexPositions[vertexPosIndex - 1], vertexNormals[vertexNormIndex - 1], textureCoordinates[texCoordIndex - 1]});
        }

        for (int i = 1; i < face.vertexIndices.size() - 1; ++i)
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
