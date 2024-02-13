#pragma once

#include <glad/glad.h>
#include <vr/State/Shader.h>

#include <string>

namespace vr {

#define MATERIAL_TEXTURES_BASE_SLOT 0
#define TEXTURES_BASE_SLOT 3
#define MAX_TEXTURES 10

class Texture {
   public:
    Texture();

    /**
     * @brief Creates a texture
     *
     * @param image The path to the image file
     * @param isMaterialTexture Boolean indicating if the texture is a material texture
     * @param slot The texture slot to use
     * @param texType The type of texture
     * @param pixelType The type of pixel
     * @return bool true if the texture was created successfully, false otherwise
     */
    bool create(const char* image, bool isMaterialTexture, unsigned int slot = 0, GLenum texType = GL_TEXTURE_2D, GLenum pixelType = GL_UNSIGNED_BYTE);

    /**
     * @brief Checks if the texture is valid
     *
     * @return bool true if the texture is valid, false otherwise
     */
    bool isValid();

    ~Texture();

    /// Assigns a texture unit to a texture
    void texUnit(GLuint program, const char* uniform, GLuint unit);

    /// Binds a texture
    void bind();

    /// Unbinds a texture
    void unbind();

    /// Deletes a texture
    void cleanup();

   private:
    GLuint m_id;
    GLenum m_type;
    bool m_valid;
    GLuint m_textureSlot;
};
}  // namespace vr
