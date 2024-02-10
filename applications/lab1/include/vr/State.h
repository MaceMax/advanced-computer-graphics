
#pragma once

#include "vr/Light.h"
#include "vr/Material.h"
#include "vr/Texture.h"

namespace vr {
class State {
   public:
    // Default constructor
    State(std::shared_ptr<Shader> shader = nullptr) : m_shader(shader), lightingEnabled(-1), cullFaceEnabled(-1) {}
    // + operator overload. Combines two states into one. Returns a shared pointer to the new state.
    std::shared_ptr<State> operator+(const State& other) const;
    State& operator+=(const State& other);

    void setLightingEnabled(bool enabled);
    bool LightingEnabled();

    void setCullFaceEnabled(bool enabled);
    bool CullFaceEnabled();

    void addLight(std::shared_ptr<Light> light);
    void setLightEnabled(size_t idx, bool enabled);
    LightVector const getLights();

    void setMaterial(std::shared_ptr<Material> material);
    std::shared_ptr<Material> const getMaterial();

    void setTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> const getTexture();

    void setShader(std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> const& getShader();

    void apply();

   private:
    int lightingEnabled;
    int cullFaceEnabled;

    LightVector m_lights;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Shader> m_shader;
};
}  // namespace vr