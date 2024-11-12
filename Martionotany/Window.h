#pragma once
#include "Monitor.h"

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    screenDim = ivec2(width, height);
    FramebufferResize();
}

GENERIC_SYSTEM(WindowInit, windowInit, Before, Start)
{
    glfwInit();

    mainMonitor.monitor = glfwGetPrimaryMonitor();
    mainMonitor.FindWorkArea();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DECORATED, windowDecorated);
    glfwWindowHint(GLFW_FLOATING, windowFloating);

    //screenDim.x = mainMonitor.workScale.x;
    //screenDim.y = mainMonitor.workScale.y / 5;
    window = glfwCreateWindow(screenDim.x, screenDim.y, "Martionotany", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    //glfwSetWindowPos(window, 0, mainMonitor.workScale.y - screenDim.y);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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