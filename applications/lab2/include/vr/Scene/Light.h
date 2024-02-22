#pragma once

#include <vr/State/Shader.h>
#include <vr/State/Texture.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

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
     */
    void apply(std::shared_ptr<vr::Shader> shader, size_t idx);

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
     * @brief Initilize the depth map texture for the light
     *
     * @param slot The texture slot to bind the depth map to
     */
    void initDepthMap(unsigned int slot);

    /**
     * @brief Set the attenuation of the light
     *
     * @param constant The constant attenuation
     * @param linear The linear attenuation
     * @param quadratic The quadratic attenuation
     */
    void setAttenuation(float constant, float linear, float quadratic);

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
    glm::mat4 m_projection;
    glm::mat4 m_view;
    Texture m_depthMap;

    friend class Scene;
};
typedef std::vector<std::shared_ptr<Light> > LightVector;

}  // namespace vr