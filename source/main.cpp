#include <iostream>
#include <random>
#include <map>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

#include "camera.h"
#include "utility/utility_functions.h"
#include "assets/import_functions.h"
#include "geometry/geometry_functions.h"
#include "resource_manager.h"
#include "geometry/model.h"

using Shading::ShaderProgram;
using Geometry::Model;

constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 900;
constexpr int MSAA = 16;
int screenWidth = SCREEN_WIDTH;
int screenHeight = SCREEN_HEIGHT;

namespace MainFunctions
{
    // Scenes
    void EmptyScene(GLFWwindow *window, ResourceManager& resourceManager);
    void SpaceScene(GLFWwindow *window, ResourceManager& resourceManager);
    void Playground(GLFWwindow *window, ResourceManager& resourceManager);
    void GeometryHousesScene(GLFWwindow *window, ResourceManager& resourceManager);
    void ModelViewer(GLFWwindow *window, ResourceManager& resourceManager);

    // Input stuff
    void ProcessInput(GLFWwindow* window);
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // Rendering stuff
    void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void SetupFramebuffer();
    void CleanupFramebuffer();
}

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

glm::mat4 view;
glm::mat4 projection;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

unsigned int framebuffer, msaaFramebuffer, renderbuffer, textureColorbuffer, msaaTextureColorbuffer;

float deltaTime = 0;
float previousTime = 0;

int main()
{
    GLFWwindow* window = Utility::SetupGLFWWindow(SCREEN_WIDTH, SCREEN_HEIGHT, MSAA, "Mars Engine");

    if (window == nullptr || Utility::InitializeGLADLoader() < 0)
        return -1;

    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(window, MainFunctions::FramebufferSizeCallback);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_MULTISAMPLE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MainFunctions::MouseCallback);
    glfwSetScrollCallback(window, MainFunctions::ScrollCallback);

    ResourceManager shaderManager = ResourceManager();

    MainFunctions::Playground(window, shaderManager);

    glfwTerminate();
    return 0;
}

void MainFunctions::EmptyScene(GLFWwindow *window, ResourceManager &resourceManager)
{
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::SpaceScene(GLFWwindow *window, ResourceManager &resourceManager)
{
    ShaderProgram* unlitShader = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/lighting/simple_diffuse_unlit.frag",
        { Matrices });
    ShaderProgram* instancedUnlitShader = resourceManager.CreateShaderProgram(
        "shaders/general/default_instanced.vert",
        "shaders/lighting/simple_diffuse_unlit.frag",
        { Matrices });

    Model planet = resourceManager.LoadModel("assets/models/planet/planet.obj");
    Model asteroid = resourceManager.LoadModel("assets/models/rock/rock.obj");
    resourceManager.SetMaterials(unlitShader);
    resourceManager.SetMaterials(instancedUnlitShader);

    camera.Position = glm::vec3(0.0f, 0.0f, 40.0f);
    planet.scale = glm::vec3(4.0f, 4.0f, 4.0f);

    std::random_device randomDevice;
    std::mt19937 randomEngine(randomDevice());
    std::uniform_int_distribution scaleDist(0, 19);
    std::uniform_int_distribution rotationDist(0, 359);

    unsigned int amount = 200000;
    glm::mat4* modelMatrices = new glm::mat4[amount];
    float radius = 120.0f;
    float offset = 50.0f;

    for (unsigned int i = 0; i < amount; ++i)
    {
        float angle = static_cast<float>(i) / static_cast<float>(amount) * 360.0f;
        glm::mat4 model = glm::mat4(1.0f);
        std::uniform_int_distribution positionDist(0, static_cast<int>(2 * offset * 100));

        float displacement = positionDist(randomEngine) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = positionDist(randomEngine) / 100.0f - offset;
        float y = displacement * 0.02f;
        displacement = positionDist(randomEngine) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = translate(model, glm::vec3(x, y, z));

        float scale = scaleDist(randomEngine) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));

        float rotAngle = rotationDist(randomEngine) % 360;
        model = rotate(model, rotAngle, glm::vec3(0.4, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }

    asteroid.SetupInstancing(amount, modelMatrices);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 500.0f);

        resourceManager.SetMatrices(view, projection);

        unlitShader->Use();
        planet.Draw(unlitShader);

        instancedUnlitShader->Use();
        asteroid.DrawInstanced();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete[] modelMatrices;
}

void MainFunctions::Playground(GLFWwindow *window, ResourceManager& resourceManager)
{
    ShaderProgram* objectShader         = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/lighting/blinn-phong/point_lights.frag",
        { Matrices, PointLights });
    ShaderProgram* windowShader         = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/transparent_texture.frag",
        { Matrices });
    ShaderProgram* solidColorShader     = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/solid_color.frag",
        { Matrices });
    ShaderProgram* reflectionShader     = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/skybox_reflection.frag",
        { Matrices });
    ShaderProgram* refractionShader     = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/general/skybox_refraction.frag",
        { Matrices });
    ShaderProgram* skyboxShader         = resourceManager.CreateShaderProgram(
        "shaders/general/skybox.vert",
        "shaders/general/skybox.frag",
        { Matrices });
    ShaderProgram* screenSpaceShader    = resourceManager.CreateShaderProgram(
        "shaders/post_processing/default_screen_space.vert",
        "shaders/post_processing/default_screen_space.frag");

    resourceManager.lightManager.AddPointLight(glm::vec3(0.0f),
                                             glm::vec3(0.03f), glm::vec3(0.5f), glm::vec3(1.0f),
                                             1.0f, 0.09f, 0.032f);
    resourceManager.lightManager.AddPointLight(glm::vec3(0.0f),
                                             glm::vec3(0.03f), glm::vec3(0.5f), glm::vec3(1.0f),
                                             1.0f, 0.09f, 0.032f);
    resourceManager.lightManager.AddPointLight(glm::vec3(-15.0f, -1.0f, -15.0f),
                                             glm::vec3(0.03f), glm::vec3(0.5f),glm::vec3(1.0f),
                                             1.0f, 0.09f, 0.032f);

    SetupFramebuffer();
    unsigned int drawBuffer = MSAA > 0 ? msaaFramebuffer : framebuffer;

    unsigned int windowVAO, windowVBO, windowEBO, windowIndicesCount, windowTexture;
    Geometry::CreateSquare(0.5f, windowVAO, windowVBO, windowEBO, windowIndicesCount);
    windowTexture = Assets::LoadTexture("assets/textures/window.png", GL_SRGB_ALPHA, GL_RGBA, GL_CLAMP_TO_EDGE);

    unsigned int screenVAO, screenVBO, screenEBO, screenIndicesCount;
    Geometry::CreateSquare(1.0f, screenVAO, screenVBO, screenEBO, screenIndicesCount);

    unsigned int skyboxVAO, skyboxTexture;
    Geometry::CreateSkyboxCube(skyboxVAO);

    std::vector<std::string> skyboxFaces
    {
        "assets/textures/yokohama/right.jpg",
        "assets/textures/yokohama/left.jpg",
        "assets/textures/yokohama/top.jpg",
        "assets/textures/yokohama/bottom.jpg",
        "assets/textures/yokohama/front.jpg",
        "assets/textures/yokohama/back.jpg"
    };
    skyboxTexture = Assets::LoadCubemap(skyboxFaces, GL_RGB, GL_RGB);

    std::vector<glm::vec3> windowObjects;
    windowObjects.emplace_back(0.0f, -1.0f, -5.0f);
    windowObjects.emplace_back(0.0f, -1.0f,  7.0f);

    Model backpack = resourceManager.LoadModel("assets/models/backpack/backpack.obj");
    Model floor = resourceManager.LoadModel("assets/models/floor/floor.obj");

    int textureCount = resourceManager.GetTextureCount();
    screenSpaceShader->Use();
    screenSpaceShader->SetInt("screenTexture", textureCount);
    glActiveTexture(GL_TEXTURE0 + textureCount++);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

    windowShader->Use();
    windowShader->SetInt("texture1", textureCount);
    glActiveTexture(GL_TEXTURE0 + textureCount++);
    glBindTexture(GL_TEXTURE_2D, windowTexture);

    resourceManager.SetMaterials(objectShader);
    floor.position = glm::vec3(0.0f, -3.5f, 0.0f);

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FRAMEBUFFER_SRGB);

    while (!glfwWindowShouldClose(window))
    {
        /*
        *
        * MAIN DRAW PASS
        * MAIN DRAW PASS
        * MAIN DRAW PASS
        *
        */
        glBindFramebuffer(GL_FRAMEBUFFER, drawBuffer);
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glStencilMask(0x00);

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        /*
         * Common shader setup
         */
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        resourceManager.lightManager.MovePointLight(0, glm::vec3(cos(currentTime / 3.25f) * 3.0f, 0, sin(currentTime / 3.25f) * 3.0f));
        resourceManager.lightManager.MovePointLight(1, glm::vec3(cos(currentTime / 1.5f) * 3.0f, sin(currentTime / 1.5f) * 3.0f, 0));
        resourceManager.UpdateLightsBuffer(view);

        /*
         * Draw solid objects
         */
        objectShader->Use();

        backpack.Draw(objectShader);
        floor.Draw(objectShader);

        /*
         * Draw environment-mapped objects
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
        resourceManager.lightManager.DrawPointLightCubes(solidColorShader);

        glEnable(GL_CULL_FACE);

        /*
        * Draw skybox
        */
        glDepthFunc(GL_LEQUAL);

        skyboxShader->Use();

        resourceManager.SetViewMatrix(glm::mat4(glm::mat3(view)));
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        resourceManager.SetViewMatrix(view);

        glDepthFunc(GL_LESS);

        /*
        * Draw transparent objects
        */
        windowShader->Use();

        glBindVertexArray(windowVAO);
        glDisable(GL_CULL_FACE);

        std::map<float, glm::vec3> sorted;
        for (glm::vec3 windowObject : windowObjects)
        {
            float distance = glm::length(camera.Position - windowObject);
            sorted[distance] = windowObject;
        }

        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = translate(model, it->second);
            model = scale(model, glm::vec3(3.0f));

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
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        if constexpr (MSAA > 0)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFramebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
            glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(screenVAO);
        screenSpaceShader->Use();

        glDrawElements(GL_TRIANGLES, screenIndicesCount, GL_UNSIGNED_INT, nullptr);

        glEnable(GL_CULL_FACE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    CleanupFramebuffer();
}

void MainFunctions::GeometryHousesScene(GLFWwindow* window, ResourceManager& resourceManager)
{
    ShaderProgram* pointsShader    = resourceManager.CreateShaderProgram(
        "shaders/general/point_houses.vert",
        "shaders/general/point_houses.geom",
        "shaders/general/point_houses.frag");

    float points[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ProcessInput(window);

        pointsShader->Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::ModelViewer(GLFWwindow *window, ResourceManager &resourceManager)
{
    ShaderProgram* objectShader = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/lighting/simple_diffuse_unlit.frag",
        { Matrices, PointLights });

    stbi_set_flip_vertically_on_load(true);
    Model loadedModel = resourceManager.LoadModel("assets/models/shanalotte/Shanalotte.obj");
    loadedModel.scale = glm::vec3(0.2f);
    resourceManager.SetMaterials(objectShader);

    resourceManager.lightManager.AddPointLight(glm::vec3(0.0f),
                                             glm::vec3(0.05f), glm::vec3(0.5f), glm::vec3(1.0f),
                                             1.0f, 0.014f, 0.0007f);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);
        resourceManager.SetMatrices(view, projection);

        resourceManager.lightManager.MovePointLight(0, camera.Position);
        resourceManager.UpdateLightsBuffer(view);

        /*
         * Draw shapes
         */
        objectShader->Use();
        loadedModel.Draw(objectShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::ProcessInput(GLFWwindow* window)
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

void MainFunctions::MouseCallback(GLFWwindow* window, const double xpos, const double ypos)
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

void MainFunctions::ScrollCallback(GLFWwindow* window, double xoffset, const double yoffset)
{
    camera.ProcesMouseScroll(yoffset);
}

void MainFunctions::FramebufferSizeCallback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);

    screenWidth = width;
    screenHeight = height;

    CleanupFramebuffer();
    SetupFramebuffer();
}

void MainFunctions::SetupFramebuffer()
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    if constexpr (MSAA <= 0)
    {
        glGenRenderbuffers(1, &renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGenFramebuffers(1, &msaaFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, msaaFramebuffer);

    glGenTextures(1, &msaaTextureColorbuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaTextureColorbuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA, GL_SRGB, screenWidth, screenHeight, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaTextureColorbuffer, 0);

    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainFunctions::CleanupFramebuffer()
{
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &renderbuffer);

    if constexpr (MSAA <= 0)
        return;

    glDeleteFramebuffers(1, &msaaFramebuffer);
    glDeleteTextures(1, &msaaTextureColorbuffer);
}