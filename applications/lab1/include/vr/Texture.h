#pragma once

#include <glad/glad.h>
#include <vr/Shader.h>

#include <string>

namespace vr {

#define MATERIAL_TEXTURES_BASE_SLOT 0
#define TEXTURES_BASE_SLOT 3
#define MAX_TEXTURES 10

class Texture {
   public:
    /// <summary>
    /// Constructor for an image
    /// </summary>
    /// <param name="image">path to an image on disk</param>
    /// <param name="texType"></param>
    /// <param name="slot">texture slot (default 0)</param>
    /// <param name="format"></param>
    /// <param name="pixelType"></param>
    Texture();
    bool create(const char* image, bool isMaterialTexture, unsigned int slot = 0, GLenum texType = GL_TEXTURE_2D, GLenum pixelType = GL_UNSIGNED_BYTE);

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
