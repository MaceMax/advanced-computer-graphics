#pragma once

#include <memory>

#include "vr/State/Shader.h"
#include "vr/State/Texture.h"

namespace vr {

#define GBUFFER_POSITION 0
#define GBUFFER_NORMAL 1
#define GBUFFER_ALBEDO 2
#define GBUFFER_SPECULAR 3
#define GBUFFER_DEPTH 4
#define GBUFFER_METALLIC_ROUGHNESS 5

class Gbuffer {
   public:
    /**
     * @brief Create a new Gbuffer
     *
     * @param width the width of the framebuffer textures
     * @param height the height of the framebuffer textures
     */
    Gbuffer(unsigned int width, unsigned int height);

    /**
     * @brief Rescale the framebuffer textures
     *
     * @param width the new width of the framebuffer textures
     * @param height the new height of the framebuffer textures
     */
    void rescaleFramebuffer(int width, int height);

    void bindFBO();
    void unbindFBO();

    void bindTextures();
    void unbindTextures();

    std::shared_ptr<Texture> getPosition() { return m_position; }
    std::shared_ptr<Texture> getNormal() { return m_normal; }
    std::shared_ptr<Texture> getAlbedo() { return m_albedo; }
    std::shared_ptr<Texture> getDepth() { return m_depth; }
    std::shared_ptr<Texture> getMetallicRoughness() { return m_metallicRoughness; }

   private:
    GLuint fbo;
    GLuint depthRbo;

    std::shared_ptr<Texture> m_position;
    std::shared_ptr<Texture> m_normal;
    std::shared_ptr<Texture> m_albedo;
    std::shared_ptr<Texture> m_depth;
    std::shared_ptr<Texture> m_metallicRoughness;
};
}  // namespace vr