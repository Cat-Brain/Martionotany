#pragma once
#include "ECSManager.h"

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    screenDim = ivec2(width, height);
    FramebufferResize();
}

GENERIC_SYSTEM(WindowInit, windowInit, Before, Start)
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

GENERIC_SYSTEM(WindowTerminate, windowTerminate, After, Close)
{
    glfwTerminate();
}

GENERIC_SYSTEM(WindowUpdate, windowUpdate, Before, Update)
{
    float newTime = static_cast<float>(glfwGetTime());
    deltaTime = newTime - currentTime;
    currentTime = newTime;

    glfwSwapBuffers(window);
}