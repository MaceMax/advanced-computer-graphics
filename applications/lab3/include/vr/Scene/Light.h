#pragma once

#include <vr/State/Shader.h>
#include <vr/State/Texture.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "vr/BoundingBox.h"

namespace vr {

/// Simple class that store light properties and apply them to Uniforms
class Light {
   public:
    /**
     * @brief Construct a new Light
     *
     * @param position  The position of the light. If the w component is 0, the light is directional, otherwise it is positional
     * @param ambient  The ambient color of the light
     * @param diffuse The diffuse color of the light
     * @param specular The specular color of the light
     */
    Light(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular);

    /**
     * @brief Apply the light to the shader
     *
     * @param shader The shader to apply the light to
     * @param idx The index of the light in the shader
     * @param shadowEnabled True if shadow mapping is enabled, false otherwise
     */
    void apply(std::shared_ptr<vr::Shader> shader, size_t idx, bool shadowEnabled);

    /**
     * @brief Set the enabled state of the light
     *
     * @param enabled True if the light is enabled, false otherwise
     */
    void setEnabled(bool enabled);

    /**
     * @brief Set the Transform matrix of the light
     *
     * @param model The transform matrix
     */
    void setTransform(glm::mat4 model) { m_model = model; }

    /**
     * @brief Get the Transform matrix of the light
     *
     * @return glm::mat4 The transform matrix
     */
    glm::mat4 getTransform() const { return m_model; }

    /**
     * @brief Set the position of the light
     *
     * @param position The position of the light
     */
    void setPosition(glm::vec4 position);

    /**
     * @brief Returns the position of the light.
     *
     * @return glm::vec4  The position of the light
     */
    glm::vec4 getPosition() const { return position; }

    /**
     * @brief Set the ambient color of the light
     *
     * @param ambient The ambient color of the light
     */
    void setAmbient(glm::vec4 ambient);

    /**
     * @brief Set the diffuse color of the light
     *
     * @param diffuse The diffuse color of the light
     */
    void setDiffuse(glm::vec4 diffuse);

    /**
     * @brief Set the specular color of the light
     *
     * @param specular The specular color of the light
     */
    void setSpecular(glm::vec4 specular);

    /**
     * @brief Get the depth map of the light
     *
     * @return const Texture& The depth map
     */
    const Texture& getDepthMap();

    /**
     * @brief Toggle the enabled state of the light
     */
    void toggleEnabled();

    /**
     * @brief Initilize the depth map texture for the light and
     *  the view and projection matrices for shadow mapping
     *
     * @param slot The texture slot to bind the depth map to
     * @param sceneBounds The bounding box of the scene. Does not include the ground plane.
     * @param groundRadius The radius of the scene bounding box which includes the ground plane.
     */
    void init(unsigned int slot, BoundingBox sceneBounds, float groundRadius);

    /**
     * @brief Set the attenuation of the light
     *
     * @param constant The constant attenuation
     * @param linear The linear attenuation
     * @param quadratic The quadratic attenuation
     */
    void setAttenuation(float constant, float linear, float quadratic);

    /**
     * @brief Update the view and projection matrices for shadow mapping
     */
    void updateShadowMatrices();

    /**
     * @brief Set shadow mapping parameters
     *
     * @param sceneRadius The radius of the scene bounding box
     * @param sceneCenter The center of the scene bounding box
     * @param farPlane The far plane of the shadow map
     */
    void setShadowParams(float sceneRadius, glm::vec3 sceneCenter, float farPlane);

    /**
     * @brief Returns the shadow matrix for the light at the given index
     *
     * @param idx The index of the shadow matrix
     * @return glm::mat4 The shadow matrix
     */
    glm::mat4 getShadowMatrix(size_t idx) const { return m_shadowMatrices[idx]; }

    /**
     * @brief Returns the far plane of the shadow map
     *
     * @return float The far plane
     */
    float getFarPlane() const { return m_farPlane; }

    glm::mat4 getProjection() const { return m_projection; }
    void setProjection(const glm::mat4& projection) { m_projection = projection; }

    glm::mat4 getView() const { return m_view; }
    void setView(const glm::mat4& view) { m_view = view; }

   private:
    bool enabled;
    glm::mat4 m_model;
    glm::vec4 position;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;

    // Shadow mapping
    float m_sceneRadius;
    glm::vec3 m_sceneCenter;
    float m_farPlane;
    glm::mat4 m_projection;
    glm::mat4 m_view;
    // Used for point lights
    std::vector<glm::mat4> m_shadowMatrices;
    Texture m_depthMap;

    std::string constructPrefix(const std::string& prefix, size_t idx, const std::string& suffix = "");

    friend class Scene;
};
typedef std::vector<std::shared_ptr<Light> > LightVector;

}  // namespace vr