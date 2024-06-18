#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

#include "utility_functions.h"

using std::vector;
using std::string;

float Utility::Clamp(const float d, const float min, const float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

GLFWwindow* Utility::SetupGLFWWindow(const int windowWith, const int windowHeight, const int msaa, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (msaa > 0)
    {
        glfwWindowHint(GLFW_SAMPLES, msaa);
    }

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
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    return 0;
}