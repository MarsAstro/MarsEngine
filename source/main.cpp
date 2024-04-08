#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "functions.h"
#include "classes/shader.h"
#include "classes/camera.h"
#include "classes/model.h"
#include "classes/point_light_collection.h"

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;
int screenWidth = SCREEN_WIDTH;
int screenHeight = SCREEN_HEIGHT;

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void setup_framebuffer();
void cleanup_framebuffer();

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

unsigned int framebuffer, renderbuffer, textureColorbuffer;

float deltaTime = 0;
float previousTime = 0;

int main()
{
    GLFWwindow* window = setupGLFWwindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mars Engine");

    if (window == nullptr || initializeGLADLoader() < 0)
        return -1;

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    stbi_set_flip_vertically_on_load(true);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    Shader objectShader = Shader("Shaders/default.vert", "Shaders/BP_pointLights.frag");
    Shader windowShader = Shader("Shaders/default.vert", "Shaders/alphatexture.frag");
    Shader solidColorShader = Shader("Shaders/default.vert", "Shaders/solidColor.frag");
    Shader skyboxShader = Shader("Shaders/skybox.vert", "Shaders/skybox.frag");
    Shader reflectionShader = Shader("Shaders/default.vert", "Shaders/skyboxReflection.frag");
    Shader refractionShader = Shader("Shaders/default.vert", "Shaders/skyboxRefraction.frag");
    Shader screenSpaceShader = Shader("Shaders/defaultScreenSpace.vert", "Shaders/postProcess.frag");

    std::vector<Shader*> uniformBlockShaders { &objectShader, &windowShader, &solidColorShader, &skyboxShader, &reflectionShader, &refractionShader };
    Shader::bindUniformBuffer(uniformBlockShaders, "Matrices", 0);

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    Model backpack = Model("Models/Backpack/backpack.obj");
    Model floor = Model("Models/Floor/floor.obj");
    floor.position = glm::vec3(0.0f, -3.5f, 0.0f);

    unsigned int windowVAO, windowVBO, windowEBO, windowIndicesCount, windowTexture;
    createViewportSquare(0.5f, windowVAO, windowVBO, windowEBO, windowIndicesCount);
    windowTexture = loadTexture("Textures/window.png", GL_RGBA, GL_RGBA, GL_CLAMP_TO_EDGE);

    unsigned int screenVAO, screenVBO, screenEBO, screenIndicesCount;
    createViewportSquare(1.0f, screenVAO, screenVBO, screenEBO, screenIndicesCount);

    unsigned int skyboxVAO, skyboxTexture;
    createSkyboxCube(skyboxVAO);

    vector<string> skyboxFaces
            {
                    "Textures/Yokohama/right.jpg",
                    "Textures/Yokohama/left.jpg",
                    "Textures/Yokohama/top.jpg",
                    "Textures/Yokohama/bottom.jpg",
                    "Textures/Yokohama/front.jpg",
                    "Textures/Yokohama/back.jpg"
            };
    stbi_set_flip_vertically_on_load(false);
    skyboxTexture = loadCubemap(skyboxFaces, GL_RGB, GL_RGB);
    stbi_set_flip_vertically_on_load(true);

    PointLightCollection pointLightCollection = PointLightCollection(
            glm::vec3(1.0f, 1.0f, 1.0f),
            0.05f, 0.5f, 1.0f,
            1.0f, 0.045f, 0.0075f
    );

    pointLightCollection.addNewLightAtPosition(glm::vec3(1.2f, 1.0f, 2.0f));
    pointLightCollection.addNewLightAtPosition(glm::vec3(1.2f, 1.0f, 2.0f));
    pointLightCollection.addNewLightAtPosition(glm::vec3(-15.0f, -1.0f, -15.0f));

    vector<glm::vec3> windows;
    windows.push_back(glm::vec3(0.0f, -1.0f, -5.0f));
    windows.push_back(glm::vec3(0.0f, -1.0f,  7.0f));

    setup_framebuffer();

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

        processInput(window);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        Shader::fillMatricesUniformBuffer(uboMatrices, view, projection);

        /*
        * Draw solid objects
        */
        objectShader.use();
        objectShader.setFloat("material.shininess", 64.0f);

        pointLightCollection.lights[0].position = glm::vec3(cos(currentTime / 3.25f) * 3.0f, 0, sin(currentTime / 3.25f) * 3.0f);
        pointLightCollection.lights[1].position = glm::vec3(cos(currentTime / 1.5f) * 3.0f, sin(currentTime / 1.5f) * 3.0f, 0);
        pointLightCollection.updateShader(objectShader, view);

        backpack.draw(objectShader);
        floor.draw(objectShader);

        /*
        * Draw environment mapped objects
        */
        reflectionShader.use();
        reflectionShader.setVec3("cameraPos", camera.Position);

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        backpack.position = glm::vec3(-14.0f, 1.0f, -12.0f);
        backpack.draw(reflectionShader);

        refractionShader.use();
        refractionShader.setVec3("cameraPos", camera.Position);

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        backpack.position = glm::vec3(-8.0f, 1.0f, -12.0f);
        backpack.draw(refractionShader);
        backpack.position = glm::vec3(0.0f);

        /*
        * Draw lightcubes
        */
        glDisable(GL_CULL_FACE);

        solidColorShader.use();
        pointLightCollection.DrawAll(solidColorShader);

        glEnable(GL_CULL_FACE);

        /*
        * Draw skybox
        */
        glDepthFunc(GL_LEQUAL);

        skyboxShader.use();

        Shader::setViewMatrixUniformBuffer(uboMatrices, glm::mat4(glm::mat3(view)));
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        Shader::setViewMatrixUniformBuffer(uboMatrices, view);

        glDepthFunc(GL_LESS);

        /*
        * Draw transparent objects
        */
        windowShader.use();
        windowShader.setInt("texture1", 0);

        glBindVertexArray(windowVAO);
        glDisable(GL_CULL_FACE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, windowTexture);

        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }

        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            model = glm::scale(model, glm::vec3(3.0f));

            windowShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, windowIndicesCount, GL_UNSIGNED_INT, 0);
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

        screenSpaceShader.use();
        glBindVertexArray(screenVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glDrawElements(GL_TRIANGLES, screenIndicesCount, GL_UNSIGNED_INT, 0);

        glEnable(GL_CULL_FACE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup_framebuffer();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool tripleSpeed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime, tripleSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime, tripleSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime, tripleSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime, tripleSpeed);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    screenWidth = width;
    screenHeight = height;

    cleanup_framebuffer();
    setup_framebuffer();
}

void setup_framebuffer()
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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

void cleanup_framebuffer()
{
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
}