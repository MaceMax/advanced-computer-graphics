#pragma once
#define CheckOpenGLError() GlCheckError_(__FILE__, __LINE__)
#define TEXTURE_PATH "../resources/textures/"
#define MATERIAL_PATH "../resources/materials/"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <dirent.h>
#include <sys/types.h>

#include <cstddef>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum ShaderProgram { PHONG = 0,
                     FLAT = 1,
                     GOURAUD = 2,
                     WIREFRAME = 3,
                     TOON = 4,
                     SKYBOX = 5,
                     WORLD_PLANE = 6,
};

enum class LightType { DIRECTIONAL = 0,
                       POINT = 1,
                       SPOT = 2 };

inline const char* LightTypeToString(LightType type) noexcept {
    switch (type) {
        case LightType::DIRECTIONAL:
            return "Directional";
        case LightType::POINT:
            return "Point";
        case LightType::SPOT:
            return "Spot";
        default:
            return "Unknown";
    }
}

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 color;
};

inline bool operator==(const Vertex& v1, const Vertex& v2) {
    return v1.position == v2.position && v1.normal == v2.normal && v1.texCoords == v2.texCoords;
}

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

// Hash function for glm::vec3
namespace std {
template <>
struct hash<glm::vec3> {
    size_t operator()(glm::vec3 const& vec) const {
        size_t h1 = hash<float>()(vec.x);
        size_t h2 = hash<float>()(vec.y);
        size_t h3 = hash<float>()(vec.z);
        return (h1 ^ (h2 << 1)) ^ h3;
    }
};

// Hash function for glm::vec2
template <>
struct hash<glm::vec2> {
    size_t operator()(glm::vec2 const& vec) const {
        size_t h1 = hash<float>()(vec.x);
        size_t h2 = hash<float>()(vec.y);
        return h1 ^ (h2 << 1);
    }
};
}  // namespace std

// Hash function for Vertex
struct VertexHasher {
    size_t operator()(const Vertex& vertex) const {
        // Compute the hash code for the vertex
        // This is a simple example, you might want to use a better hash function
        return std::hash<glm::vec3>()(vertex.position) ^
               std::hash<glm::vec3>()(vertex.normal) ^
               std::hash<glm::vec2>()(vertex.texCoords) ^
               std::hash<glm::vec3>()(vertex.color);
    }
};

struct Material {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    std::string ambientMapPath;
    std::string diffuseMapPath;
    std::string specularMapPath;
    std::string normalMapPath;
};

enum ProjectionType { PERSPECTIVE,
                      ORTHOGRAPHIC };

inline GLenum GlCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

inline GLuint LoadTexture(const std::string& path, std::string directory) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#define BUFFER_OFFSET(i) (reinterpret_cast<char*>(0 + (i)))

/*
 * Usage:
 * string s = "foo";
 * DEBUG( "Value of s is '" << s << "'");
 * -> DBG: Value of s is 'foo'
 */
#define DEBUG(msg) \
    std::cerr << "DBG " << __FILE__ << "(" << __LINE__ << "): " << msg << std::endl

/*
 * Usage:
 * string s = "foo";
 * DEBUGVAR( s );
 * -> DBG: testfile.cpp(20) s = foo
 */
#define DEBUGVAR(var) \
    std::cerr << "DBG " << __FILE__ << "(" << __LINE__ << "): " << #var << " = " << (var) << std::endl
