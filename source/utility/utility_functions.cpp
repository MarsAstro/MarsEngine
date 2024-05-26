#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

#include "stb_image.h"
#include "utility_functions.h"

using std::vector;
using std::string;

float Utility::Clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

void Utility::CreateSquare(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount)
{
    fillLevel = Clamp(fillLevel, 0.0f, 1.0f);

    float vertices[] =
            {
                    // positions				   // normals		   // tex coords
                    fillLevel, -fillLevel, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,  // bottom right
                    -fillLevel, -fillLevel, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,  // bottom left
                    fillLevel,  fillLevel, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,  // top right
                    -fillLevel,  fillLevel, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f   // top left
            };

    unsigned int indices[] =
            {
                    0, 1, 2,
                    1, 2, 3
            };

    indicesCount = 6;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Utility::CreateCube(float sideLength, unsigned int& VAO, unsigned int& VBO)
{
    float vertices[] = {
            -sideLength, -sideLength, -sideLength, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
            sideLength, -sideLength, -sideLength, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
            sideLength,  sideLength, -sideLength, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
            sideLength,  sideLength, -sideLength, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
            -sideLength,  sideLength, -sideLength, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
            -sideLength, -sideLength, -sideLength, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

            -sideLength, -sideLength,  sideLength, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
            sideLength, -sideLength,  sideLength, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
            sideLength,  sideLength,  sideLength, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
            sideLength,  sideLength,  sideLength, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
            -sideLength,  sideLength,  sideLength, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
            -sideLength, -sideLength,  sideLength, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

            -sideLength,  sideLength,  sideLength, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            -sideLength,  sideLength, -sideLength, -1.0f, 0.0f,  0.0f, 1.0f, 1.0f,
            -sideLength, -sideLength, -sideLength, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
            -sideLength, -sideLength, -sideLength, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
            -sideLength, -sideLength,  sideLength, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
            -sideLength,  sideLength,  sideLength, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,

            sideLength,  sideLength,  sideLength, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            sideLength,  sideLength, -sideLength, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
            sideLength, -sideLength, -sideLength, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            sideLength, -sideLength, -sideLength, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            sideLength, -sideLength,  sideLength, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            sideLength,  sideLength,  sideLength, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

            -sideLength, -sideLength, -sideLength, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
            sideLength, -sideLength, -sideLength, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
            sideLength, -sideLength,  sideLength, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
            sideLength, -sideLength,  sideLength, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
            -sideLength, -sideLength,  sideLength, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
            -sideLength, -sideLength, -sideLength, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

            -sideLength,  sideLength, -sideLength, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
            sideLength,  sideLength, -sideLength, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
            sideLength,  sideLength,  sideLength, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
            sideLength,  sideLength,  sideLength, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
            -sideLength,  sideLength,  sideLength, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
            -sideLength,  sideLength, -sideLength, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Utility::CreateSkyboxCube(unsigned int& VAO)
{
    float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Utility::CreateTriangle(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount)
{
    fillLevel = Clamp(fillLevel, 0.0f, 1.0f);

    float vertices[] =
    {
        // positions				   // colors		  // tex coords
        fillLevel, -fillLevel, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,	// bottom right
        -fillLevel, -fillLevel, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,	// bottom left
        0.0,        fillLevel, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f,	// top mid
    };

    unsigned int indices[] =
    {
        0, 1, 2
    };

    indicesCount = sizeof(indices);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

unsigned int Utility::LoadTexture(const char* texturePath, GLenum internalFormat, GLenum outputFormat, GLenum wrapFormat)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFormat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFormat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, outputFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    return texture;
}

unsigned int Utility::LoadCubemap(std::vector<std::string> faces, GLenum internalFormat, GLenum outputFormat)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D
                    (
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                            0, internalFormat, width, height, 0, outputFormat, GL_UNSIGNED_BYTE, data
                    );

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

GLFWwindow* Utility::SetupGLFWWindow(int windowWith, int windowHeight, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWith, windowHeight, title, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    return window;
}

int Utility::InitializeGLADLoader()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    return 0;
}