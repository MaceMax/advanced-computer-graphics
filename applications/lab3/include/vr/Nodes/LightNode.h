#pragma once

#include "Node.h"
#include "vr/Scene/Light.h"

namespace vr {

class LightNode : public Node {
   public:
    /**
     * @brief Constructs a new Light Node
     *
     * @param name The name of the node
     */
    LightNode(std::shared_ptr<Light> light, const std::string& name = "LightNode") : m_light(light), Node(name) {}
    virtual void accept(NodeVisitor& visitor) override;
    // Return empty bounding box
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) override;
    std::shared_ptr<Light> getLight() { return m_light; }

   private:
    std::shared_ptr<Light> m_light;
};

}  // namespace vr