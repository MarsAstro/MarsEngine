#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

using std::vector;
using std::string;

float clamp(float d, float min, float max);

void createViewportSquare(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount);
void createOriginCube(float sideLength, unsigned int& VAO, unsigned int& VBO);
void createSkyboxCube(unsigned int& VAO);
void createViewportTriangle(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount);
unsigned int loadTexture(const char* texturePath, GLenum internalFormat, GLenum outputFormat, GLenum wrapFormat);
unsigned int loadCubemap(vector<string> faces, GLenum internalFormat, GLenum outputFormat);

GLFWwindow* setupGLFWwindow(int windowWith, int windowHeight, const char* title);
int initializeGLADLoader();
