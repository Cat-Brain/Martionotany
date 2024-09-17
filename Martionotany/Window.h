#pragma once
#include "ECSManager.h"

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screenDim = ivec2(width, height);
}

void WindowInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenDim.x, screenDim.y, "Martionotany", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, screenDim.x, screenDim.y);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
}

void WindowTerminate()
{
    glfwTerminate();
}

void WindowUpdate()
{
    float newTime = static_cast<float>(glfwGetTime());
    deltaTime = newTime - currentTime;
    currentTime = newTime;

    glfwSwapBuffers(window);

    glClearColor(0.3f, 0.3f, 0.6f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}