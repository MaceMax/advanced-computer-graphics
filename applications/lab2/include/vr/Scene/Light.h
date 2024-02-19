#pragma once

#include <vr/State/Shader.h>

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
     * @brief Set the position of the light
     *
     * @param position The position of the light
     */
    void setPosition(glm::vec4 position);

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
     * @brief Set the attenuation of the light
     *
     * @param constant The constant attenuation
     * @param linear The linear attenuation
     * @param quadratic The quadratic attenuation
     */
    void setAttenuation(float constant, float linear, float quadratic);

   private:
    bool enabled;
    glm::vec4 position;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;

    friend class Scene;
};
typedef std::vector<std::shared_ptr<Light> > LightVector;

}  // namespace vr