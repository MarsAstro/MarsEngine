#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace Utility
{
    float Clamp(float d, float min, float max);

    void CreateSquare(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount);
    void CreateCube(float sideLength, unsigned int& VAO, unsigned int& VBO);
    void CreateSkyboxCube(unsigned int& VAO);
    void CreateTriangle(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount);
    unsigned int LoadTexture(const char* texturePath, GLenum internalFormat, GLenum outputFormat, GLenum wrapFormat);
    unsigned int LoadCubemap(std::vector<std::string> faces, GLenum internalFormat, GLenum outputFormat);

    GLFWwindow* SetupGLFWWindow(int windowWith, int windowHeight, const char* title);
    int InitializeGLADLoader();
}