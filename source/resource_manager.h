#pragma once

#include <vector>
#include <memory>
#include "shading/shader_program.h"
#include "shading/lighting/light_manager.h"

enum ShaderUniformBlock
{
    Matrices = 0,
    PointLights = 1,
    Materials = 2
};

class ResourceManager
{
public:
    ResourceManager();

    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* fragmentPath, std::initializer_list<ShaderUniformBlock> uniformBlocks);
    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* geometryPath, const char* fragmentPath, std::initializer_list<ShaderUniformBlock> uniformBlocks);

    void SetMatrices(const glm::mat4 &view, const glm::mat4 &projection) const;
    void SetViewMatrix(glm::mat4 view) const;
    void UpdateLightsBuffer(const glm::mat4& viewMatrix) const;

private:
    static const char* GetUniformBlockLayoutName(ShaderUniformBlock uniformBlock);

    std::vector<std::unique_ptr<Shading::ShaderProgram>> mShaderProgramList;

    unsigned int mUBOMatrices;
    unsigned int mUBOPointLights;

    const unsigned int MATRICES_COUNT = 2;
    const unsigned int MAX_POINT_LIGHTS = 64;
    const unsigned int SIZEOF_POINT_LIGHT = 20 * sizeof(float);
    const unsigned int POINT_LIGHTS_ARRAY_SIZE = MAX_POINT_LIGHTS * SIZEOF_POINT_LIGHT;

public:
    Shading::Lighting::LightManager lightManager;
};