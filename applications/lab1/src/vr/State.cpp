#include <vr/State.h>
#include <vr/glErrorUtil.h>

#include <iostream>

using namespace vr;

// Parent state + child state = new state
std::shared_ptr<State> State::operator+(const State& childState) const {
    std::shared_ptr<State> newState = std::make_shared<State>();
    // Child state has precedence. If child state has a value, use it. Otherwise, use parent state's value.

    if (childState.lightingEnabled != -1) {
        newState->lightingEnabled = childState.lightingEnabled;
    } else {
        newState->lightingEnabled = lightingEnabled;
    }

    if (childState.cullFaceEnabled != -1) {
        newState->cullFaceEnabled = childState.cullFaceEnabled;
    } else {
        newState->cullFaceEnabled = cullFaceEnabled;
    }

    if (childState.m_shader != nullptr) {
        newState->setShader(childState.m_shader);
    } else {
        newState->setShader(m_shader);
    }

    if (!childState.m_lights.empty()) {
        newState->m_lights = childState.m_lights;
    } else {
        newState->m_lights = m_lights;
    }

    if (childState.m_material != nullptr) {
        newState->m_material = childState.m_material;
    } else {
        newState->m_material = m_material;
    }

    if (childState.m_texture != nullptr) {
        newState->m_texture = childState.m_texture;
    } else {
        newState->m_texture = m_texture;
    }

    return newState;
}

State& State::operator+=(const State& other) {
    if (other.lightingEnabled != -1) {
        lightingEnabled = other.lightingEnabled;
    }

    if (other.cullFaceEnabled != -1) {
        cullFaceEnabled = other.cullFaceEnabled;
    }

    if (other.m_shader != nullptr) {
        m_shader = other.m_shader;
    } else {
        m_shader = m_shader;
    }

    if (!other.m_lights.empty()) {
        m_lights = other.m_lights;
    }

    if (other.m_material != nullptr) {
        m_material = other.m_material;
    }

    if (other.m_texture != nullptr) {
        m_texture = other.m_texture;
    }

    return *this;
}

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

void State::addLight(std::shared_ptr<Light> light) {
    m_lights.push_back(light);
}

void State::setLightEnabled(size_t idx, bool enabled) {
    m_lights[idx]->setEnabled(enabled);
}

LightVector const State::getLights() {
    return m_lights;
}

void State::setMaterial(std::shared_ptr<Material> material) {
    m_material = material;
}

std::shared_ptr<Texture> const State::getTexture() {
    return m_texture;
}

void State::setTexture(std::shared_ptr<Texture> texture) {
    m_texture = texture;
}

std::shared_ptr<Material> const State::getMaterial() {
    return m_material;
}

void State::setShader(std::shared_ptr<Shader> shader) {
    m_shader = shader;
}

std::shared_ptr<Shader> const& State::getShader() {
    return m_shader;
}

void State::apply() {
    if (m_shader == nullptr)
        return;

    m_shader->use();
    // Update number of lights
    m_shader->setInt("numberOfLights", m_lights.size());

    // Turn lighting off in shader since if two two geometries share the same shader and one of them has lighting disabled,
    // the other one will also have lighting disabled.
    m_shader->setInt("lightingEnabled", lightingEnabled);

    // Apply lightsources
    size_t i = 0;
    if (m_lights.size() != 0 && lightingEnabled) {
        for (auto l : m_lights) {
            l->apply(m_shader, i);
            i++;
        }
    }

    if (m_material != nullptr)
        m_material->apply(m_shader);

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