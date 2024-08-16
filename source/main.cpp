#include <iostream>
#include <random>
#include <map>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

#include "constants.h"
#include "camera.h"
#include "utility/utility_functions.h"
#include "assets/import_functions.h"
#include "geometry/geometry_functions.h"
#include "resource_manager.h"
#include "geometry/model.h"
#include "scenes/scene.h"

using Shading::ShaderProgram;
using Geometry::Model;

int screenWidth = Constants::SCREEN_WIDTH;
int screenHeight = Constants::SCREEN_HEIGHT;
float lastX = Constants::SCREEN_WIDTH / 2.0f;
float lastY = Constants::SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
bool cameraLock = false;
bool canToggleCameraLock = true;

unsigned int framebuffer, msaaFramebuffer, renderbuffer, textureColorbuffer, msaaTextureColorbuffer;

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
glm::mat4 view;
glm::mat4 projection;

float previousTime = 0;
float deltaTime = 0;

namespace MainFunctions
{
    // Scenes
    void EmptyScene(GLFWwindow* window, ResourceManager& resourceManager);
    void GrassScene(GLFWwindow* window, ResourceManager& resourceManager);
    void VertexColors(GLFWwindow *window, ResourceManager &resourceManager);
    void CelShader(GLFWwindow* window, ResourceManager& resourceManager);
    void LightingShaderDev(GLFWwindow* window, ResourceManager& resourceManager);
    void ScreenShader(GLFWwindow* window, ResourceManager& resourceManager);
    void SpaceScene(GLFWwindow* window, ResourceManager& resourceManager);
    void Playground(GLFWwindow* window, ResourceManager& resourceManager);
    void ShadowsScene(GLFWwindow* window, ResourceManager& resourceManager);
    void GeometryHousesScene(GLFWwindow* window, ResourceManager& resourceManager);
    void ModelViewer(GLFWwindow* window, ResourceManager& resourceManager);

    // Input stuff
    void ProcessInput(GLFWwindow* window);
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // Rendering stuff
    void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void SetupFramebuffer();
    void CleanupFramebuffer();
}

int main()
{
    GLFWwindow* window = Utility::SetupGLFWWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, Constants::MSAA, "Mars Engine");

    if (window == nullptr || Utility::InitializeGLADLoader() < 0)
        return -1;

    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(window, MainFunctions::FramebufferSizeCallback);
    glViewport(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    glEnable(GL_MULTISAMPLE);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MainFunctions::MouseCallback);
    glfwSetScrollCallback(window, MainFunctions::ScrollCallback);

    ResourceManager shaderManager = ResourceManager();

    MainFunctions::GrassScene(window, shaderManager);

    glfwTerminate();
    return 0;
}

void MainFunctions::EmptyScene(GLFWwindow *window, ResourceManager &resourceManager)
{
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::GrassScene(GLFWwindow *window, ResourceManager &resourceManager)
{
    ShaderProgram* skysphereShader = resourceManager.CreateShaderProgram(
        "shaders/shaderdev/skysphere.vert",
        "shaders/shaderdev/skysphere.frag",
        { Matrices });
    ShaderProgram* groundShader = resourceManager.CreateShaderProgram(
        "shaders/shaderdev/ground.vert",
        "shaders/shaderdev/ground.frag",
        { Matrices });

    const int GRASS_COUNT = 16;
    const int GRASS_SEGMENTS = 6;
    const int GRASS_VERTICES = (GRASS_SEGMENTS + 1) * 2;
    const int GRASS_PATCH_SIZE = 10;
    const float GRASS_WIDTH = 0.25;
    const float GRASS_HEIGHT = 2;

    unsigned int grassVAO, grassIndicesCount;
    Geometry::CreateGrassGeometry(GRASS_SEGMENTS, grassVAO, grassIndicesCount);

    camera = Camera(glm::vec3(0.0f, 3.5f, 15.0f));
    Model skysphere = resourceManager.LoadModel("assets/shapes/inverse_sphere.obj");
    Model ground = resourceManager.LoadModel("assets/shapes/plane.obj");
    ground.scale = glm::vec3(6.0);
    skysphere.scale = glm::vec3(50.0);

    unsigned int gridSquare = Assets::LoadTexture("assets/textures/square.png", GL_SRGB_ALPHA, GL_RGB, GL_REPEAT);
    groundShader->Use();
    groundShader->SetInt("diffuseTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gridSquare);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        ShaderProgram grassShader = ShaderProgram("shaders/shaderdev/grass.vert", "shaders/shaderdev/grass.frag");

        int returnValue;
        glGetProgramiv(grassShader.mID, GL_ACTIVE_UNIFORM_BLOCKS, &returnValue);

        int uniformBlockIndex = glGetUniformBlockIndex(grassShader.mID, "Matrices");
        glUniformBlockBinding(grassShader.mID, uniformBlockIndex, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        skysphereShader->Use();
        skysphereShader->SetVec2("resolution", screenWidth, screenHeight);
        skysphere.Draw(skysphereShader);

        groundShader->Use();
        glBindTexture(GL_TEXTURE_2D, gridSquare);
        ground.Draw(groundShader);

        grassShader.Use();
        grassShader.SetVec4("grassParams", glm::vec4(GRASS_SEGMENTS, GRASS_VERTICES, GRASS_WIDTH, GRASS_HEIGHT));
        grassShader.SetMat4("model", glm::mat4(1.0));
        grassShader.SetFloat("time", currentTime);
        grassShader.SetVec2("resolution", screenWidth, screenHeight);

        glBindVertexArray(grassVAO);
        glDrawElementsInstanced(GL_TRIANGLES, grassIndicesCount, GL_UNSIGNED_INT, nullptr, GRASS_COUNT);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::VertexColors(GLFWwindow *window, ResourceManager &resourceManager)
{
    ShaderProgram* skyboxShader         = resourceManager.CreateShaderProgram(
    "shaders/general/skybox.vert",
    "shaders/general/skybox.frag",
    { Matrices });

    unsigned int windowVAO, windowVBO, windowEBO, windowIndicesCount;
    Geometry::CreateSquare(1.0f, windowVAO, windowVBO, windowEBO, windowIndicesCount);

    unsigned int skyboxVAO;
    Geometry::CreateSkyboxCube(skyboxVAO);

    std::vector<std::string> skyboxFaces
    {
        "assets/textures/ocean_mountains/right.jpg",
        "assets/textures/ocean_mountains/left.jpg",
        "assets/textures/ocean_mountains/top.jpg",
        "assets/textures/ocean_mountains/bottom.jpg",
        "assets/textures/ocean_mountains/front.jpg",
        "assets/textures/ocean_mountains/back.jpg"
    };
    unsigned int skyboxTexture = Assets::LoadCubemap(skyboxFaces, GL_SRGB, GL_RGB);

    Model suzanne = resourceManager.LoadModel("assets/shapes/cube.obj");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);

    while (!glfwWindowShouldClose(window))
    {
        ShaderProgram objectShader = ShaderProgram(
            "shaders/shaderdev/vertex_colors.vert", "shaders/shaderdev/vertex_colors.frag");

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        objectShader.Use();
        objectShader.SetFloat("time", currentTime);
        suzanne.Draw(&objectShader);

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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::CelShader(GLFWwindow *window, ResourceManager &resourceManager)
{
    ShaderProgram* skyboxShader         = resourceManager.CreateShaderProgram(
        "shaders/general/skybox.vert",
        "shaders/general/skybox.frag",
    { Matrices });

    unsigned int windowVAO, windowVBO, windowEBO, windowIndicesCount;
    Geometry::CreateSquare(1.0f, windowVAO, windowVBO, windowEBO, windowIndicesCount);

    unsigned int skyboxVAO;
    Geometry::CreateSkyboxCube(skyboxVAO);

    std::vector<std::string> skyboxFaces
    {
        "assets/textures/ocean_mountains/right.jpg",
        "assets/textures/ocean_mountains/left.jpg",
        "assets/textures/ocean_mountains/top.jpg",
        "assets/textures/ocean_mountains/bottom.jpg",
        "assets/textures/ocean_mountains/front.jpg",
        "assets/textures/ocean_mountains/back.jpg"
    };
    unsigned int skyboxTexture = Assets::LoadCubemap(skyboxFaces, GL_SRGB, GL_RGB);

    Model suzanne = resourceManager.LoadModel("assets/shapes/suzanne.obj");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);

    while (!glfwWindowShouldClose(window))
    {
        ShaderProgram objectShader = ShaderProgram(
            "shaders/shaderdev/model.vert", "shaders/shaderdev/cel_shading.frag");

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        objectShader.Use();
        objectShader.SetFloat("time", currentTime);
        suzanne.Draw(&objectShader);

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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::LightingShaderDev(GLFWwindow *window, ResourceManager &resourceManager)
{
    ShaderProgram* skyboxShader         = resourceManager.CreateShaderProgram(
        "shaders/general/skybox.vert",
        "shaders/general/skybox.frag",
    { Matrices });

    unsigned int windowVAO, windowVBO, windowEBO, windowIndicesCount;
    Geometry::CreateSquare(1.0f, windowVAO, windowVBO, windowEBO, windowIndicesCount);

    unsigned int skyboxVAO;
    Geometry::CreateSkyboxCube(skyboxVAO);

    std::vector<std::string> skyboxFaces
    {
        "assets/textures/ocean_mountains/right.jpg",
        "assets/textures/ocean_mountains/left.jpg",
        "assets/textures/ocean_mountains/top.jpg",
        "assets/textures/ocean_mountains/bottom.jpg",
        "assets/textures/ocean_mountains/front.jpg",
        "assets/textures/ocean_mountains/back.jpg"
    };
    unsigned int skyboxTexture = Assets::LoadCubemap(skyboxFaces, GL_SRGB, GL_RGB);

    Model suzanne = resourceManager.LoadModel("assets/shapes/suzanne.obj");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);

    while (!glfwWindowShouldClose(window))
    {
        ShaderProgram objectShader = ShaderProgram(
            "shaders/shaderdev/model.vert", "shaders/shaderdev/lighting.frag");

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        objectShader.Use();
        objectShader.SetFloat("time", currentTime);
        suzanne.Draw(&objectShader);

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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainFunctions::ScreenShader(GLFWwindow *window, ResourceManager &resourceManager)
{
    unsigned int windowVAO, windowVBO, windowEBO, windowIndicesCount;
    Geometry::CreateSquare(1.0f, windowVAO, windowVBO, windowEBO, windowIndicesCount);

    stbi_set_flip_vertically_on_load(true);
    unsigned int diffuse1 = Assets::LoadTexture("assets/textures/flower.jpg", GL_SRGB, GL_RGB, GL_REPEAT);
    unsigned int diffuse2 = Assets::LoadTexture("assets/textures/flower.jpg", GL_SRGB, GL_RGB, GL_REPEAT);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);

    while (!glfwWindowShouldClose(window))
    {
        ShaderProgram windowShader = ShaderProgram(
            "shaders/shaderdev/screen_space.vert", "shaders/shaderdev/cloudy_day.frag");

        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);

        windowShader.Use();
        windowShader.SetInt("diffuse1", 0);
        windowShader.SetInt("diffuse2", 1);
        windowShader.SetVec2("resolution", screenWidth, screenHeight);
        windowShader.SetFloat("time", currentTime);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, diffuse2);

        glBindVertexArray(windowVAO);
        glDrawElements(GL_TRIANGLES, windowIndicesCount, GL_UNSIGNED_INT, nullptr);

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
    resourceManager.ApplyMaterials(unlitShader);
    resourceManager.ApplyMaterials(instancedUnlitShader);

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
        "shaders/lighting/point_lights.frag",
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
    unsigned int drawBuffer = Constants::MSAA > 0 ? msaaFramebuffer : framebuffer;

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
    skyboxTexture = Assets::LoadCubemap(skyboxFaces, GL_SRGB, GL_RGB);

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
    glActiveTexture(GL_TEXTURE0 + textureCount);
    glBindTexture(GL_TEXTURE_2D, windowTexture);

    resourceManager.ApplyMaterials(objectShader);
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
        resourceManager.UpdatePointLightsBuffer(view);

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

        if constexpr (Constants::MSAA > 0)
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

void MainFunctions::ShadowsScene(GLFWwindow *window, ResourceManager& resourceManager)
{
    ShaderProgram* objectShader         = resourceManager.CreateShaderProgram(
        "shaders/general/default.vert",
        "shaders/lighting/directional_light.frag",
        { Matrices });
    ShaderProgram* skyboxShader         = resourceManager.CreateShaderProgram(
        "shaders/general/skybox.vert",
        "shaders/general/skybox.frag",
        { Matrices });
    ShaderProgram* lightDepthShader     = resourceManager.CreateShaderProgram(
            "shaders/lighting/light_space.vert",
            "shaders/lighting/light_space.frag");

    resourceManager.lightManager.SetDirectionalLight(
        glm::vec3(0.33f, -1.0f, 0.3f), glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f)
    );
    Shading::Lighting::DirectionalShadow dirShadow = resourceManager.lightManager.GetDirectionalShadow();

    unsigned int skyboxVAO, skyboxTexture;
    Geometry::CreateSkyboxCube(skyboxVAO);

    std::vector<std::string> skyboxFaces
    {
        "assets/textures/ocean_mountains/right.jpg",
        "assets/textures/ocean_mountains/left.jpg",
        "assets/textures/ocean_mountains/top.jpg",
        "assets/textures/ocean_mountains/bottom.jpg",
        "assets/textures/ocean_mountains/front.jpg",
        "assets/textures/ocean_mountains/back.jpg"
    };
    skyboxTexture = Assets::LoadCubemap(skyboxFaces, GL_SRGB, GL_RGB);
    Model model = resourceManager.LoadModel("assets/models/rock/rock.obj");
    Model floor = resourceManager.LoadModel("assets/models/floor/floor.obj");

    Scene scene;
    scene.AddObject(&floor, glm::vec3(0.0f, -3.5f, 0.0f));
    scene.AddObject(&model, glm::vec3(0.0f));
    scene.AddObject(&model, glm::vec3(0.0f, 3.0f, -2.0f));
    scene.AddObject(&model, glm::vec3(4.0f, 0.0f, 3.0f));

    int textureCount = resourceManager.GetTextureCount();
    objectShader->Use();
    objectShader->SetInt("shadowMap", textureCount);
    glActiveTexture(GL_TEXTURE0 + textureCount);
    glBindTexture(GL_TEXTURE_2D, dirShadow.depthMapTexture);

    resourceManager.ApplyMaterials(objectShader);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        ProcessInput(window);

        /*
         *
         * SHADOWS DEPTH PASS
         * SHADOWS DEPTH PASS
         * SHADOWS DEPTH PASS
         *
         */
        glViewport(0, 0, Constants::SHADOW_WIDTH, Constants::SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, dirShadow.depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        float nearPlane = 1.0f;
        float farPlane = 20.0f;

        view        = lookAt(dirShadow.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection  = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

        glm::mat4 lightSpaceMatrix = projection * view;
        lightDepthShader->Use();
        lightDepthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glCullFace(GL_FRONT);
        scene.DrawScene(lightDepthShader);
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);

        /*
        *
        * MAIN DRAW PASS
        * MAIN DRAW PASS
        * MAIN DRAW PASS
        *
        */
        /*
         * Common shader setup
         */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

        resourceManager.SetMatrices(view, projection);
        resourceManager.UpdateDirectionalLight(objectShader, view);

        /*
         * Draw solid objects
         */
        objectShader->Use();
        objectShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        scene.DrawScene(objectShader);

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
    resourceManager.ApplyMaterials(objectShader);

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
        resourceManager.UpdatePointLightsBuffer(view);

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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && canToggleCameraLock)
    {
        cameraLock = !cameraLock;
        canToggleCameraLock = false;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        canToggleCameraLock = true;
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

    if (!cameraLock)
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

    if constexpr (Constants::MSAA <= 0)
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
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Constants::MSAA, GL_SRGB, screenWidth, screenHeight, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaTextureColorbuffer, 0);

    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, Constants::MSAA, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
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

    if constexpr (Constants::MSAA <= 0)
        return;

    glDeleteFramebuffers(1, &msaaFramebuffer);
    glDeleteTextures(1, &msaaTextureColorbuffer);
}