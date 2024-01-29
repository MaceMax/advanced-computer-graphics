#include <vr/State.h>
#include <vr/glErrorUtil.h>

#include <iostream>

using namespace vr;

// Parent state + child state = new state
std::shared_ptr<State> State::operator+(const State& childState) const {
    std::shared_ptr<State> newState = std::make_shared<State>(m_shader);
    // Child state has precedence. If child state has a value, use it. Otherwise, use parent state's value.
    newState->lightingEnabled = childState.lightingEnabled;
    newState->cullFaceEnabled = childState.cullFaceEnabled;

    if (childState.m_shader != nullptr) {
        newState->setShader(childState.m_shader);
    } else {
        newState->setShader(m_shader);
    }

    if (childState.m_lights != nullptr) {
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
    lightingEnabled = other.lightingEnabled;
    cullFaceEnabled = other.cullFaceEnabled;

    if (other.m_shader != nullptr) {
        m_shader = other.m_shader;
    } else {
        m_shader = m_shader;
    }

    if (other.m_lights != nullptr) {
        m_lights = other.m_lights;
    } else {
        m_lights = m_lights;
    }

    if (other.m_material != nullptr) {
        m_material = other.m_material;
    } else {
        m_material = m_material;
    }

    if (other.m_texture != nullptr) {
        m_texture = other.m_texture;
    } else {
        m_texture = m_texture;
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
    if (m_lights == nullptr) {
        m_lights = std::make_shared<LightVector>();
    }
    m_lights->push_back(light);
}

void State::setLightEnabled(size_t idx, bool enabled) {
    m_lights->at(idx)->enabled = enabled;
}

void State::setMaterial(std::shared_ptr<Material> material) {
    m_material = material;
}

void State::setTexture(std::shared_ptr<Texture> texture) {
    m_texture = texture;
}

void State::setShader(std::shared_ptr<Shader> shader) {
    m_shader = shader;
}

std::shared_ptr<Shader> const State::getShader() {
    return m_shader;
}

void State::apply() {
    if (m_shader == nullptr)
        return;

    m_shader->use();
    // Update number of lights
    // std::cout << "Number of lights: " << m_lights->size() << std::endl;
    m_shader->setInt("numberOfLights", m_lights->size());
    CHECK_GL_ERROR_LINE_FILE();
    m_shader->setBool("lightingEnabled", lightingEnabled);
    CHECK_GL_ERROR_LINE_FILE();

    if (m_lights != nullptr) {
        for (size_t i = 0; i < m_lights->size(); i++) {
            if (lightingEnabled) {
                m_lights->at(i)->apply(m_shader, i);
            }
        }
    }

    if (m_material != nullptr) {
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