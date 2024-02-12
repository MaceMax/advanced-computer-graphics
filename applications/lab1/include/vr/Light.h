#pragma once

#include <vr/Shader.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace vr {

/// Simple class that store light properties and apply them to Uniforms
class Light {
   public:
    Light(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular);
    void apply(std::shared_ptr<vr::Shader> shader, size_t idx);
    void setEnabled(bool enabled);
    void setPosition(glm::vec4 position);
    void setAmbient(glm::vec4 ambient);
    void setDiffuse(glm::vec4 diffuse);
    void setSpecular(glm::vec4 specular);
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