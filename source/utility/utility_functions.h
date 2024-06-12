#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace Utility
{
    float Clamp(float d, float min, float max);

    GLFWwindow* SetupGLFWWindow(int windowWith, int windowHeight, int msaa, const char* title);
    int InitializeGLADLoader();
}