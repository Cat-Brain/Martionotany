#pragma once
#include "ECS.h"

class Shader
{
public:
    static constexpr int bufferSize = 1000;
    static char sourceBuffer[bufferSize];
    static vector<Shader*> shaders;

    uint program;
    string vertLocation, fragLocation;

    Shader(string vertLocation, string fragLocation) :
        program(0), vertLocation(vertLocation + ".txt"), fragLocation(fragLocation + ".txt")
    {
        shaders.push_back(this);
    }
};
char Shader::sourceBuffer[Shader::bufferSize] = { 0 };
vector<Shader*> Shader::shaders = { };

Shader defaultShader("DefaultVert", "DefaultFrag");


void ShaderInit()
{
    std::ifstream testStream;

    for (Shader* shader : Shader::shaders)
    {
        std::fill_n(Shader::sourceBuffer, Shader::bufferSize, '\0');
        testStream.open(shader->vertLocation);
        testStream.read(Shader::sourceBuffer, Shader::bufferSize);
        testStream.close();

        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        char* bufferPointer = &Shader::sourceBuffer[0];
        glShaderSource(vertexShader, 1, &bufferPointer, NULL);
        glCompileShader(vertexShader);

        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        std::fill_n(Shader::sourceBuffer, Shader::bufferSize, '\0');
        testStream.open(shader->fragLocation);
        testStream.read(Shader::sourceBuffer, Shader::bufferSize);
        testStream.close();

        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &bufferPointer, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        shader->program = glCreateProgram();

        glAttachShader(shader->program, vertexShader);
        glAttachShader(shader->program, fragmentShader);
        glLinkProgram(shader->program);

        glGetProgramiv(shader->program, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader->program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
}

void ShaderTerminate()
{
    for (Shader* shader : Shader::shaders)
        glDeleteProgram(shader->program);
}