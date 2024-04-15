#pragma once

#include <vector>
#include <memory>
#include "shader.h"

enum ShaderUniformBlock
{
    Matrices = 0,
    PointLights = 1
};

class ShaderManager
{
public:
    ShaderManager();

    static const char* GetUniformBlockLayoutName(ShaderUniformBlock uniformBlock);

    Shader* CreateShader(const char* vertexPath, const char* fragmentPath);
    Shader* CreateShader(const char* vertexPath, const char* fragmentPath, std::initializer_list<ShaderUniformBlock> uniformBlocks);

    void SetViewAndProjectionMatrices(glm::mat4 view, glm::mat4 projection) const;
    void SetViewMatrix(glm::mat4 view) const;
    void SetProjectionMatrix(glm::mat4 projection) const;

private:
    std::vector<std::unique_ptr<Shader>> shaderList;

    unsigned int uboMatrices;
};
