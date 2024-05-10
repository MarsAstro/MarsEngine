#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int mID;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void Use() const;

    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;

    void SetVec2(const std::string& name, glm::vec2 value) const;
    void SetVec2(const std::string& name, float x, float y) const;

    void SetVec3(const std::string& name, glm::vec3 value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;

    void SetVec4(const std::string& name, glm::vec4 value) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w) const;

    void SetMat4(const std::string& name, glm::mat4 matrix);
};