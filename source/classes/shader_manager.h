#pragma once

#include <vector>
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
    ~ShaderManager();

    static const char* GetUniformBlockLayoutName(ShaderUniformBlock uniformBlock);

    Shader* CreateShader(const char* vertexPath, const char* fragmentPath);
    Shader* CreateShader(const char* vertexPath, const char* fragmentPath, std::initializer_list<ShaderUniformBlock> uniformBlocks);

private:
    std::vector<Shader*> shader_list;
    unsigned int uboMatrices;
};
