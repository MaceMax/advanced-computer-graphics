#pragma once
#include "vr/Node.h"
#include "vr/NodeVisitor.h"

namespace vr {

class Group : public Node {
   public:
    Group(const std::string& name = "Group") : Node(name) {}
    virtual void accept(NodeVisitor& visitor) override;
    void addChild(std::shared_ptr<Node> node);
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) override;

    NodeVector& getChildren();

   protected:
    NodeVector m_children;

    // NOTE: State should be inherited from parent i.e. Node
};

}  // namespace vr