#include <vr/State.h>

using namespace vr;

void State::setLightingEnabled(bool enabled) {
    lightingEnabled = enabled;
}

bool State::LightingEnabled() {
    return lightingEnabled;
}

void State::setCullFaceEnabled(bool enabled) {
    cullFaceEnabled = enabled;
}

bool State::CullFaceEnabled() {
    return cullFaceEnabled;
}

void State::setLights(std::shared_ptr<LightVector> lights) {
    m_lights = lights;
}

void State::setLightEnabled(size_t idx, bool enabled) {
    m_lights->at(idx)->enabled = enabled;
}

void State::setMaterial(std::shared_ptr<Material> material) {
    m_material = material;
}

void State::apply() {
    m_shader->use();
    m_shader->setBool("lightingEnabled", lightingEnabled);

    for (size_t i = 0; i < m_lights->size(); i++) {
        if (lightingEnabled) {
            m_lights->at(i)->apply(m_shader, i);
        }
    }

    if (m_material) {
        m_material->apply(m_shader);
    }

    if (cullFaceEnabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    /*
    if (m_texture) {
        m_texture->apply(m_shader);
    }
    */
}