#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "functions.h"
#include "classes/camera.h"
#include "classes/model.h"
#include "classes/point_light_collection.h"
#include "classes/shader_manager.h"

using Shading::ShaderManager;
using Shading::ShaderProgram;

constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 900;
int screenWidth = SCREEN_WIDTH;
int screenHeight = SCREEN_HEIGHT;

void ProcessInput(GLFWwindow* window);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void SetupFramebuffer();
void CleanupFramebuffer();

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

unsigned int framebuffer, renderbuffer, textureColorbuffer;

float deltaTime = 0;
float previousTime = 0;

int main()
{
    GLFWwindow* window = SetupGLFWWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mars Engine");

    if (window == nullptr || InitializeGLADLoader() < 0)
        return -1;

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    stbi_set_flip_vertically_on_load(true);

    ShaderManager shaderManager = ShaderManager();

    ShaderProgram* objectShader         = shaderManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/lighting/blinn-phong/point_lights.frag",
        { Shading::Matrices, Shading::PointLights });
    ShaderProgram* windowShader         = shaderManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/transparent_texture.frag",
        { Shading::Matrices });
    ShaderProgram* solidColorShader     = shaderManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/solid_color.frag",
        { Shading::Matrices });
    ShaderProgram* reflectionShader     = shaderManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/skybox_reflection.frag",
        { Shading::Matrices });
    ShaderProgram* refractionShader     = shaderManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/skybox_refraction.frag",
        { Shading::Matrices });
    ShaderProgram* skyboxShader         = shaderManager.CreateShaderProgram(
        "shaders/general/skybox.vert",
        "shaders/general/skybox.frag",
        { Shading::Matrices });
    ShaderProgram* screenSpaceShader    = shaderManager.CreateShaderProgram(
        "shaders/post_processing/default_screen_space.vert",
        "shaders/post_processing/default_screen_space.frag");

    shaderManager.lightManager.AddPointLight(glm::vec3(0.0f, 0.5f, 0.5f), glm::vec3(0.05f), glm::vec3(0.5f),
        glm::vec3(1.0f), 1.0f, 0.045f, 0.0075f);

    Model backpack = Model("assets/models/backpack/backpack.obj");
    Model floor = Model("assets/models/floor/floor.obj");
    floor.position = glm::vec3(0.0f, -3.5f, 0.0f);

    unsigned int windowVAO, windowVBO, windowEBO, windowIndicesCount, windowTexture;
    CreateSquare(0.5f, windowVAO, windowVBO, windowEBO, windowIndicesCount);
    windowTexture = LoadTexture("assets/textures/window.png", GL_RGBA, GL_RGBA, GL_CLAMP_TO_EDGE);

    unsigned int screenVAO, screenVBO, screenEBO, screenIndicesCount;
    CreateSquare(1.0f, screenVAO, screenVBO, screenEBO, screenIndicesCount);

    unsigned int skyboxVAO, skyboxTexture;
    CreateSkyboxCube(skyboxVAO);

    vector<string> skyboxFaces
    {
        "assets/textures/yokohama/right.jpg",
        "assets/textures/yokohama/left.jpg",
        "assets/textures/yokohama/top.jpg",
        "assets/textures/yokohama/bottom.jpg",
        "assets/textures/yokohama/front.jpg",
        "assets/textures/yokohama/back.jpg"
    };
    stbi_set_flip_vertically_on_load(false);
    skyboxTexture = LoadCubemap(skyboxFaces, GL_RGB, GL_RGB);
    stbi_set_flip_vertically_on_load(true);

    vector<glm::vec3> windowObjects;
    windowObjects.emplace_back(0.0f, -1.0f, -5.0f);
    windowObjects.emplace_back(0.0f, -1.0f,  7.0f);

    SetupFramebuffer();

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        /*
        *
        * MAIN DRAW PASS
        * MAIN DRAW PASS
        * MAIN DRAW PASS
        *
        */
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glStencilMask(0x00);

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        shaderManager.SetViewAndProjectionMatrices(view, projection);
        shaderManager.UpdateLights(view);

        /*
        * Draw solid objects
        */
        objectShader->Use();
        objectShader->SetFloat("material.shininess", 64.0f);

        backpack.Draw(objectShader);
        floor.Draw(objectShader);

        /*
        * Draw environment mapped objects
        */
        reflectionShader->Use();
        reflectionShader->SetVec3("cameraPos", camera.Position);

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        backpack.position = glm::vec3(-14.0f, 1.0f, -12.0f);
        backpack.Draw(reflectionShader);

        refractionShader->Use();
        refractionShader->SetVec3("cameraPos", camera.Position);

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        backpack.position = glm::vec3(-8.0f, 1.0f, -12.0f);
        backpack.Draw(refractionShader);
        backpack.position = glm::vec3(0.0f);

        /*
        * Draw lightcubes
        */
        glDisable(GL_CULL_FACE);

        solidColorShader->Use();
        shaderManager.lightManager.DrawPointLightCubes(solidColorShader);

        glEnable(GL_CULL_FACE);

        /*
        * Draw skybox
        */
        glDepthFunc(GL_LEQUAL);

        skyboxShader->Use();

        shaderManager.SetViewMatrix(glm::mat4(glm::mat3(view)));
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shaderManager.SetViewMatrix(view);

        glDepthFunc(GL_LESS);

        /*
        * Draw transparent objects
        */
        windowShader->Use();
        windowShader->SetInt("texture1", 0);

        glBindVertexArray(windowVAO);
        glDisable(GL_CULL_FACE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, windowTexture);

        std::map<float, glm::vec3> sorted;
        for (glm::vec3 windowObject : windowObjects)
        {
            float distance = glm::length(camera.Position - windowObject);
            sorted[distance] = windowObject;
        }

        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            model = glm::scale(model, glm::vec3(3.0f));

            windowShader->SetMat4("model", model);
            glDrawElements(GL_TRIANGLES, windowIndicesCount, GL_UNSIGNED_INT, nullptr);
        }

        glEnable(GL_CULL_FACE);

        /*
        *
        * SCREEN SPACE DRAW PASS
        * SCREEN SPACE DRAW PASS
        * SCREEN SPACE DRAW PASS
        *
        */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenSpaceShader->Use();
        glBindVertexArray(screenVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glDrawElements(GL_TRIANGLES, screenIndicesCount, GL_UNSIGNED_INT, nullptr);

        glEnable(GL_CULL_FACE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    CleanupFramebuffer();

    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool tripleSpeed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, tripleSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, tripleSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, tripleSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, tripleSpeed);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcesMouseScroll(yoffset);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    screenWidth = width;
    screenHeight = height;

    CleanupFramebuffer();
    SetupFramebuffer();
}

void SetupFramebuffer()
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CleanupFramebuffer()
{
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
}