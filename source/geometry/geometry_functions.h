#pragma once

namespace Geometry
{
    void CreateSquare(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount);
    void CreateCube(float sideLength, unsigned int& VAO, unsigned int& VBO);
    void CreateSkyboxCube(unsigned int& VAO);
    void CreateTriangle(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount);
}
