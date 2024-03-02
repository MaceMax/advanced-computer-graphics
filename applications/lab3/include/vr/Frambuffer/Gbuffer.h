#pragma once

#include <memory>

#include "vr/State/Shader.h"
#include "vr/State/Texture.h"

namespace vr {
class Gbuffer {
   public:
    Gbuffer(unsigned int width, unsigned int height);

    void rescaleFramebuffer(int width, int height);

    void bindFBO();
    void unbindFBO();

    void bindTextures();
    void unbindTextures();

   private:
    GLuint fbo;
    GLuint depthRbo;

    std::shared_ptr<Texture> m_position;
    std::shared_ptr<Texture> m_normal;
    std::shared_ptr<Texture> m_albedo;
};
}  // namespace vr