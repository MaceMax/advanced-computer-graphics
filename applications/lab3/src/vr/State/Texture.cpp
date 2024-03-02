#include <vr/State/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <vr/FileSystem.h>
#include <vr/glErrorUtil.h>

#include <iostream>

#include "stb_image.h"
using namespace vr;

bool Texture::create(const char* image, bool isMaterialTexture, unsigned int slot, GLenum texType, GLenum pixelType) {
    if (m_valid)
        cleanup();

    std::string filepath = image;

    bool exist = vr::FileSystem::exists(filepath);

    std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
    if (vrPath.empty())
        std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;

    if (!exist && !vrPath.empty()) {
        filepath = std::string(vrPath) + "/" + filepath;
        exist = vr::FileSystem::exists(filepath);
    }
    if (!exist) {
        std::cerr << "Unable to locate image: " << image << std::endl;
        return false;
    }

    if (isMaterialTexture && slot >= TEXTURES_BASE_SLOT) {
        std::cerr << "Texture slot is greater than the maximum number of textures allowed" << std::endl;
        return false;
    }

    if (!isMaterialTexture && slot >= MAX_TEXTURES) {
        std::cerr << "Texture slot is greater than the maximum number of textures allowed" << std::endl;
        return false;
    }

    if (isMaterialTexture)
        m_textureSlot = MATERIAL_TEXTURES_BASE_SLOT + slot;
    else
        m_textureSlot = TEXTURES_BASE_SLOT + slot;

    // Assigns the type of the texture of the texture object
    m_type = texType;

    GLenum texFormat = GL_RGBA;

    // Stores the width, height, and the number of color channels of the image
    int widthImg, heightImg, numColCh;
    // Flips the image so it appears right side up
    stbi_set_flip_vertically_on_load(true);
    // Reads the image from a file and stores it in bytes
    unsigned char* bytes = stbi_load(filepath.c_str(), &widthImg, &heightImg, &numColCh, 0);
    if (!bytes) {
        std::cerr << "Error reading image: " << image << std::endl;
        return false;
    }

    if (numColCh == 3)
        texFormat = GL_RGB;

    // Generates an OpenGL texture object
    glGenTextures(1, &m_id);

    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + m_textureSlot);
    glBindTexture(texType, m_id);

    CHECK_GL_ERROR_LINE_FILE();

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Assigns the image to the OpenGL Texture object
    glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, texFormat, pixelType, bytes);
    // Generates MipMaps
    glGenerateMipmap(texType);

    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(bytes);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(texType, 0);

    m_valid = true;
    return true;
}

bool Texture::createFramebufferTexture(unsigned int width, unsigned int height, GLenum texType, GLenum pixelType) {
    if (m_valid)
        cleanup();

    m_type = texType;

    glGenTextures(1, &m_id);

    m_textureSlot = SCREEN_TEXTURE_SLOT;
    glActiveTexture(GL_TEXTURE0 + m_textureSlot);
    glBindTexture(m_type, m_id);

    glTexImage2D(m_type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(m_type, 0);
    m_valid = true;
    return true;
}

void Texture::createDepthTexture(unsigned int width, unsigned int height, unsigned int slot, bool isDirectional) {
    if (m_valid)
        cleanup();

    if (isDirectional)
        m_type = GL_TEXTURE_2D;
    else
        m_type = GL_TEXTURE_CUBE_MAP;

    glGenTextures(1, &m_id);

    m_textureSlot = DEPTH_TEXTURE_BASE_SLOT + slot;
    glActiveTexture(GL_TEXTURE0 + m_textureSlot);
    glBindTexture(m_type, m_id);

    if (isDirectional)
        glTexImage2D(m_type, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    else
        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

    glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (isDirectional) {
        glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    } else {
        glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    glBindTexture(m_type, 0);
    m_valid = true;
}

Texture::Texture() : m_id(0), m_type(0), m_valid(false), m_textureSlot(0) {
}

Texture::~Texture() {
    cleanup();
}

bool Texture::isValid() {
    return m_valid;
}

void Texture::rescale(unsigned int width, unsigned int height) {
    if (m_valid) {
        glBindTexture(m_type, m_id);
        if (m_type == GL_TEXTURE_2D)
            glTexImage2D(m_type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(m_type, 0);
    }
}

void Texture::texUnit(GLuint program, const char* uniform, unsigned int unit) {
    // Gets the location of the uniform
    GLuint texUni = glGetUniformLocation(program, uniform);

    // Sets the value of the uniform
    glUniform1i(texUni, unit);
}

void Texture::bind() {
    glActiveTexture(GL_TEXTURE0 + m_textureSlot);
    if (m_valid)
        glBindTexture(m_type, m_id);
}

void Texture::unbind() {
    if (m_valid)
        glBindTexture(m_type, m_id);
}

void Texture::cleanup() {
    if (m_valid)
        glDeleteTextures(1, &m_id);

    m_valid = false;
}