#include "model_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Utility::Mesh Utility::ModelLoader::LoadMesh(const char *path)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::ifstream obj;
    obj.exceptions(std::ifstream::badbit);

    try
    {
        obj.open(path);
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
            {
                glm::vec3 vertexPosition;
                lineStream >> lineWord;
                vertexPosition.x = std::stof(lineWord);
                lineStream >> lineWord;
                vertexPosition.y = std::stof(lineWord);
                lineStream >> lineWord;
                vertexPosition.z = std::stof(lineWord);

                Vertex vertex { vertexPosition };
                vertices.push_back(vertex);
            }

            if (lineWord == "f")
            {
                std::vector<unsigned int> faceIndices;

                while (!lineStream.eof())
                {
                    lineStream >> lineWord;
                    faceIndices.push_back(std::stoi(lineWord) - 1);
                }

                for (int i = 1; i < faceIndices.size() - 1; ++i)
                {
                    indices.push_back(faceIndices[0]);
                    indices.push_back(faceIndices[i]);
                    indices.push_back(faceIndices[i+1]);
                }
            }
        }
    }
    catch (std::ifstream::failure& exception)
    {
        std::cout << "ERROR::ASSET::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    return { vertices, indices };
}
