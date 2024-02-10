#pragma once
#include "vr/Node.h"

namespace vr {
typedef std::pair<float, std::shared_ptr<Group>> GroupPair;
class LodNode : public Node {
   public:
    LodNode(const std::string& name = "LodNode") : Node(name) {}
    virtual void accept(NodeVisitor& visitor) override;
    virtual BoundingBox calculateBoundingBox(glm::mat4 m_mat) override;
    void addChild(float maxDistance, std::shared_ptr<Group> node);
    Group& getChild(const glm::vec3& cameraPosition);
    void setMaxDistance(float maxDistance);
    void applyTransformation(const glm::mat4& matrix);

   private:
    std::vector<GroupPair>
        m_children;
    float m_maxDistance = -1.0f;
    BoundingBox bbox;
};

}  // namespace vr