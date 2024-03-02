#pragma once

#include <glad/glad.h>
#include <vr/State/Shader.h>

#include <string>

namespace vr {

#define DIFFUSE_TEXTURE 0
#define SPECULAR_TEXTURE 1
#define DISPLACEMENT_TEXTURE 2
#define NORMAL_TEXTURE 3
#define AMBIENT_OCCLUSION_TEXTURE 4
#define EMISSION_TEXTURE 5
#define METALLIC_TEXTURE 6
#define ROUGHNESS_TEXTURE 7

#define MATERIAL_TEXTURES_BASE_SLOT 0
#define TEXTURES_BASE_SLOT 9
#define MAX_TEXTURES 2
#define MAX_MATERIAL_TEXTURES 8

#define DEPTH_TEXTURE_BASE_SLOT 11
#define MAX_DEPTH_TEXTURES 10

#define G_BUFFER_POSITION_SLOT 22
#define G_BUFFER_ALBEDO_SLOT 23
#define G_BUFFER_NORMAL_SLOT 24
#define G_BUFFER_DEPTH_SLOT 25
#define G_BUFFER_METALLIC_ROUGHNESS 26
#define SCREEN_TEXTURE_SLOT 32

#define DEPTH_MAP_RESOLUTION 2048

class Texture {
   public:
    Texture();

    /**
     * @brief Creates a texture from an image file
     *
     * @param image The path to the image file
     * @param isMaterialTexture Boolean indicating if the texture is a material texture
     * @param slot The texture slot to use
     * @param texType The type of texture
     * @param pixelType The type of pixel
     * @return bool true if the texture was created successfully, false otherwise
     */
    bool create(const char* image, bool isMaterialTexture, unsigned int slot = 0, GLenum texType = GL_TEXTURE_2D, GLenum pixelType = GL_UNSIGNED_BYTE, GLint texFormat = GL_RGBA);

    /**
     * @brief Creates a texture that will be used in a framebuffer.
     *
     * @param width The slot to use
     * @param width The width of the texture
     * @param height The height of the texture
     * @param texType The type of texture
     * @param pixelType The type of pixel
     * @return bool true if the texture was created successfully, false otherwise
     */
    bool createFramebufferTexture(unsigned int slot, unsigned int width, unsigned int height, GLenum texType = GL_TEXTURE_2D);

    /**
     * @brief Creates a depth texture
     *
     * @param width the width of the texture
     * @param height the height of the texture
     * @param slot the texture slot to use
     * @param isDirectional boolean indicating if the texture is to be used for a directional light or not.
     */
    void createDepthTexture(unsigned int width, unsigned int height, unsigned int slot, bool isDirectional = true);

    /**
     * @brief Checks if the texture is valid
     *
     * @return bool true if the texture is valid, false otherwise
     */
    bool isValid();

    /**
     * @brief Rescales a texture
     *
     * @param width The new width of the texture
     * @param height The new height of the texture
     */
    void rescale(unsigned int width, unsigned int height);

    ~Texture();

    GLuint id() const { return m_id; }

    GLuint slot() const { return m_textureSlot; }

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
    GLint m_texFormat;
    GLenum m_pixelType;
    bool m_valid;
    GLuint m_textureSlot;
};
}  // namespace vr
