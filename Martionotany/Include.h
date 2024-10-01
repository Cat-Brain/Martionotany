#pragma once
// C++ STL includes:
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <format>
#include <string>
#include <typeindex>

// STL Usings:
using std::min;
using std::max;
using std::cout;
using std::vector;
using std::function;
using std::string;
using std::pair;
using std::tuple;
using std::get;
using std::type_index;

// Include Directory Includes:
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

// Include Directory Usings:
using glm::vec2;
using glm::ivec2;
using glm::vec3;
using glm::ivec3;
using glm::vec4;
using glm::ivec4;
using glm::mat2;
using glm::mat3;
using glm::mat4;

// Typedefs That I Don't Know Where to Put:

typedef unsigned int uint;
typedef unsigned short ushort;
typedef uint8_t byte;

// Global Variables That I Don't Know Where to Put:

ivec2 screenDim = ivec2(800, 600);
float currentTime = 0, deltaTime = 0;
GLFWwindow* window;
constexpr int pixelsPerUnit = 8;

namespace Vec
{
    constexpr vec2 up(0, 1), right(1, 0), down(0, -1), left(-1, 0);
}

// Random Functions That I Don't Know Where to Put:
namespace std
{
    inline std::string to_string(const vec2& a) {
        return std::format("({}, {})", a.x, a.y);
    }
}

inline std::ostream& operator<<(std::ostream& os, const vec2& a)
{
    os << std::to_string(a);
    return os;
}