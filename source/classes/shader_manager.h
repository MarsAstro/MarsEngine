#pragma once

#include <vector>
#include <memory>
#include "shader_program.h"
#include "light_manager.h"

namespace Shading
{
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

        ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
        ShaderProgram* CreateShaderProgram(const char* vertexPath, const char* fragmentPath, std::initializer_list<ShaderUniformBlock> uniformBlocks);

        void SetViewAndProjectionMatrices(const glm::mat4 &view, const glm::mat4 &projection) const;
        void SetViewMatrix(glm::mat4 view) const;
        void SetProjectionMatrix(glm::mat4 projection) const;
        void UpdatePointLights(const glm::mat4& view) const;

        Lighting::LightManager lightManager;

    private:
        std::vector<std::unique_ptr<ShaderProgram>> mShaderList;

        unsigned int mUBOMatrices;
        unsigned int mUBOPointLights;

        const unsigned int MATRICES_COUNT = 2;
        const unsigned int MAX_POINT_LIGHTS = 64;
        const unsigned int SIZEOF_POINT_LIGHT = 20 * sizeof(float);
        const unsigned int POINT_LIGHTS_ARRAY_SIZE = MAX_POINT_LIGHTS * SIZEOF_POINT_LIGHT;
    };
}