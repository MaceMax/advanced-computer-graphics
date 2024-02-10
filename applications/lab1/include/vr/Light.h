#pragma once

#include <vr/Shader.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace vr {

/// Simple class that store light properties and apply them to Uniforms
class Light {
   public:
    Light(glm::vec4 position, glm::vec4 diffuse, glm::vec4 specular);
    void apply(std::shared_ptr<vr::Shader> shader, size_t idx);
    void setEnabled(bool enabled);
    void setPosition(glm::vec4 position);
    void setDiffuse(glm::vec4 diffuse);
    void setSpecular(glm::vec4 specular);

   private:
    bool enabled;
    glm::vec4 position;
    glm::vec4 diffuse;
    glm::vec4 specular;
    friend class Scene;
    // std::shared_ptr<Mesh> m_mesh;
    void createMesh();
    // std::shared_ptr<Mesh>& getMesh();
};
typedef std::vector<std::shared_ptr<Light> > LightVector;

}  // namespace vr