#pragma once

#include "glad/glad.h"
#include <vector>
#include <string>

namespace Assets
{
    unsigned int LoadTexture(const std::string &path, const bool &isSRGB);
    unsigned int LoadTexture(const char* texturePath, GLenum internalFormat, GLenum outputFormat, GLenum wrapFormat);
    unsigned int LoadCubemap(std::vector<std::string> faces, GLenum internalFormat, GLenum format);
}
