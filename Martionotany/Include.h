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
using std::reference_wrapper;
using std::move;

// Include Directory Includes:
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include "glm/gtx/rotate_vector.hpp"
#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"
#include "macro_sequence_for.h"

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

using glm::identity;
using glm::normalize;

// Typedefs That I Don't Know Where to Put:

typedef unsigned int uint;
typedef unsigned short ushort;
typedef uint8_t byte;

typedef type_index CHash; // Component hash

// Global Variables That I Don't Know Where to Put:

ivec2 screenDim = ivec2(800, 600);
float currentTime = 0, deltaTime = 0;
GLFWwindow* window;
constexpr int pixelsPerUnit = 8;
bool inDebug = false;
bool windowDecorated = true, windowFloating = false;

namespace Vec
{
    constexpr vec2 up(0, 1), right(1, 0), down(0, -1), left(-1, 0);
}

// Useful Utility Macros:
#define DEFAULT_SF_FOR_STEP(n, d, x) d + 1
#define DEFAULT_SF_FOR_FINAL(n, d)
#define NUM_ARGS(...) (std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value)
#define FOR_EACH(body, sequence) SF_FOR_EACH(body, DEFAULT_SF_FOR_STEP, DEFAULT_SF_FOR_FINAL, 0, sequence)

// Random Functions That I Don't Know Where to Put:
namespace std
{
    inline std::string to_string(const vec4& a) {
        return std::format("({}, {}, {}, {})", a.x, a.y, a.z, a.w);
    }
    inline std::string to_string(const vec3& a) {
        return std::format("({}, {}, {})", a.x, a.y, a.z);
    }
    inline std::string to_string(const vec2& a) {
        return std::format("({}, {})", a.x, a.y);
    }
    inline std::string to_string(const ivec4& a) {
        return std::format("({}, {}, {}, {})", a.x, a.y, a.z, a.w);
    }
    inline std::string to_string(const ivec3& a) {
        return std::format("({}, {}, {})", a.x, a.y, a.z);
    }
    inline std::string to_string(const ivec2& a) {
        return std::format("({}, {})", a.x, a.y);
    }
}

inline std::ostream& operator<<(std::ostream& os, const vec4& a)
{
    os << std::to_string(a);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const vec3& a)
{
    os << std::to_string(a);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const vec2& a)
{
    os << std::to_string(a);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ivec4& a)
{
    os << std::to_string(a);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ivec3& a)
{
    os << std::to_string(a);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ivec2& a)
{
    os << std::to_string(a);
    return os;
}