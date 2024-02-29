#include <vr/Scene/Light.h>
#include <vr/glErrorUtil.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

using namespace vr;

Light::Light(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular) : enabled(true) {
    this->position = position;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->m_model = glm::mat4(1.0f);

    this->constant = 1.0f;
    this->linear = 0.09f;
    this->quadratic = 0.032f;
}

void Light::init(unsigned int slot, BoundingBox sceneBounds, float groundRadius) {
    m_depthMap.createDepthTexture(DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION, slot, position.w == 0);
    m_sceneCenter = sceneBounds.getCenter();
    m_sceneRadius = sceneBounds.getRadius();
    m_farPlane = groundRadius;
    updateShadowMatrices();
}

void Light::updateShadowMatrices() {
    glm::vec4 world_position = m_model * position;
    if (position.w == 0) {
        glm::vec3 lightDir = glm::normalize(glm::vec3(world_position));
        glm::vec3 up = glm::abs(lightDir.y) < 0.999 ? glm::vec3(0.0, 1.0, 0.0) : glm::vec3(1.0, 0.0, 0.0);
        glm::mat4 view = glm::lookAt(lightDir + m_sceneCenter, m_sceneCenter, up);
        glm::mat4 proj = glm::ortho(-m_sceneRadius, m_sceneRadius, -m_sceneRadius, m_sceneRadius, 0.1f, m_farPlane);

        m_view = view;
        m_projection = proj;
    } else {
        float aspect = (float)DEPTH_MAP_RESOLUTION / (float)DEPTH_MAP_RESOLUTION;  // 1:1 aspect ratio, but this allows for flexibility
        glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, 1.0f, 100.0f);
        m_shadowMatrices.clear();
        m_shadowMatrices.push_back(proj * glm::lookAt(glm::vec3(world_position), glm::vec3(world_position) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        m_shadowMatrices.push_back(proj * glm::lookAt(glm::vec3(world_position), glm::vec3(world_position) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        m_shadowMatrices.push_back(proj * glm::lookAt(glm::vec3(world_position), glm::vec3(world_position) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        m_shadowMatrices.push_back(proj * glm::lookAt(glm::vec3(world_position), glm::vec3(world_position) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        m_shadowMatrices.push_back(proj * glm::lookAt(glm::vec3(world_position), glm::vec3(world_position) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        m_shadowMatrices.push_back(proj * glm::lookAt(glm::vec3(world_position), glm::vec3(world_position) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        m_projection = proj;
    }
}

void Light::setShadowParams(float sceneRadius, glm::vec3 sceneCenter, float farPlane) {
    m_sceneRadius = sceneRadius;
    m_sceneCenter = sceneCenter;
    m_farPlane = farPlane == 0.0f ? 2 * m_sceneRadius : farPlane;
    updateShadowMatrices();
}

const Texture& Light::getDepthMap() {
    return m_depthMap;
}

void Light::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void Light::setPosition(glm::vec4 position) {
    this->position = position;
    updateShadowMatrices();
}

void Light::setAmbient(glm::vec4 ambient) {
    this->ambient = ambient;
}

void Light::setDiffuse(glm::vec4 diffuse) {
    this->diffuse = diffuse;
}

void Light::setSpecular(glm::vec4 specular) {
    this->specular = specular;
}

void Light::setAttenuation(float constant, float linear, float quadratic) {
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
}

void Light::apply(std::shared_ptr<vr::Shader> shader, size_t idx, bool shadowsEnabled) {
    std::string prefix = constructPrefix("lights", idx, ".");
    glm::vec4 world_position = m_model * position;

    std::string uniform_name = prefix + "enabled";
    shader->setInt(uniform_name, enabled);
    shader->setVec4(prefix + "ambient", this->ambient);
    shader->setVec4(prefix + "diffuse", this->diffuse);
    shader->setVec4(prefix + "specular", this->specular);
    shader->setVec4(prefix + "position", world_position);

    std::string lightSpacePrefix = constructPrefix("lightSpaceMatrix", idx);
    shader->setMat4(lightSpacePrefix, m_projection * m_view);

    std::string activeLightprefix = constructPrefix("activeLights", idx);
    shader->setInt(activeLightprefix, enabled);

    if (shadowsEnabled) {
        std::string depthPrefix = position.w == 0 ? constructPrefix("depthMaps", idx) : constructPrefix("cubeDepthMaps", idx);
        shader->setInt(depthPrefix, DEPTH_TEXTURE_BASE_SLOT + idx);
        m_depthMap.bind();
    }

    if (position.w != 0) {
        shader->setFloat(prefix + "constant", this->constant);
        shader->setFloat(prefix + "linear", this->linear);
        shader->setFloat(prefix + "quadratic", this->quadratic);
        shader->setFloat(prefix + "farPlane", this->m_farPlane);
    }
    CHECK_GL_ERROR_LINE_FILE();
}

std::string Light::constructPrefix(const std::string& prefix, size_t idx, const std::string& suffix) {
    return prefix + "[" + std::to_string(idx) + "]" + suffix;
}
