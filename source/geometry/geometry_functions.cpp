#include "geometry_functions.h"

#include <glad/glad.h>
#include "../utility/utility_functions.h"

void Geometry::CreateSquare(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount)
{
    fillLevel = Utility::Clamp(fillLevel, 0.0f, 1.0f);

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

void Geometry::CreateCube(float sideLength, unsigned int& VAO, unsigned int& VBO)
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

void Geometry::CreateSkyboxCube(unsigned int& VAO)
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

void Geometry::CreateTriangle(float fillLevel, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int& indicesCount)
{
    fillLevel = Utility::Clamp(fillLevel, 0.0f, 1.0f);

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