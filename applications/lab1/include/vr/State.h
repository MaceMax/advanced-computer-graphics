
#pragma once

#include "vr/Light.h"
#include "vr/Material.h"
#include "vr/Texture.h"

namespace vr {
class State {
   public:
    // Default constructor
    State(std::shared_ptr<Shader> shader) : m_shader(shader),
                                            lightingEnabled(true),
                                            cullFaceEnabled(true){};

    void setLightingEnabled(bool enabled);
    bool LightingEnabled();

    void setCullFaceEnabled(bool enabled);
    bool CullFaceEnabled();

    void setLights(std::shared_ptr<LightVector> lights);
    void setLightEnabled(size_t idx, bool enabled);

    void setMaterial(std::shared_ptr<Material> material);
    void setTexture(std::shared_ptr<Texture> texture);

    void apply();

   private:
    bool lightingEnabled;
    bool cullFaceEnabled;

    std::shared_ptr<LightVector> m_lights;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Shader> m_shader;
};
}  // namespace vr