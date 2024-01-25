
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
    // + operator overload. Combines two states into one. Returns a shared pointer to the new state.
    std::shared_ptr<State> operator+(const State& other) const;
    State& operator+=(const State& other);

    void setLightingEnabled(bool enabled);
    bool LightingEnabled();

    void setCullFaceEnabled(bool enabled);
    bool CullFaceEnabled();

    void addLight(std::shared_ptr<Light>& light);
    void setLightEnabled(size_t idx, bool enabled);

    void setMaterial(std::shared_ptr<Material> material);
    void setTexture(std::shared_ptr<Texture> texture);

    std::shared_ptr<Shader> const getShader();

    void apply();

   private:
    bool lightingEnabled;
    bool cullFaceEnabled;

    std::shared_ptr<LightVector> m_lights = nullptr;
    std::shared_ptr<Material> m_material = nullptr;
    std::shared_ptr<Texture> m_texture = nullptr;
    std::shared_ptr<Shader> m_shader = nullptr;
};
}  // namespace vr