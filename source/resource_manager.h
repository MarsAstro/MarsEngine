#pragma once

#include <vector>
#include <memory>
#include "shading/shader_program.h"
#include "shading/lighting/light_manager.h"
#include "geometry/model.h"

enum ShaderUniformBlock
{
    Matrices = 0,
    PointLights = 1
};

class ResourceManager
{
public:
    ResourceManager();

    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* fragmentPath, std::initializer_list<ShaderUniformBlock> uniformBlocks);
    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    Shading::ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* geometryPath, const char* fragmentPath, std::initializer_list<ShaderUniformBlock> uniformBlocks);

    Geometry::Model LoadModel(const char* modelPath);

    void SetMatrices(const glm::mat4 &view, const glm::mat4 &projection) const;
    void SetViewMatrix(glm::mat4 view) const;
    void ApplyMaterials(const Shading::ShaderProgram* shader) const;
    void UpdateDirectionalLight(const Shading::ShaderProgram* shader, const glm::mat4& viewMatrix) const;
    void UpdatePointLightsBuffer(const glm::mat4& viewMatrix) const;

    int GetTextureCount() const;

private:
    static const char* GetUniformBlockLayoutName(ShaderUniformBlock uniformBlock);

    std::vector<std::unique_ptr<Shading::ShaderProgram>> mShaderProgramList;
    std::vector<Geometry::Material> mMaterials;

    unsigned int mUBOMatrices;
    unsigned int mUBOPointLights;

    static constexpr unsigned int MATRICES_COUNT = 2;
    static constexpr unsigned int MAX_POINT_LIGHTS = 64;

public:
    Shading::Lighting::LightManager lightManager;
};