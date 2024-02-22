#include <vr/Nodes/LightNode.h>
#include <vr/Visitors/NodeVisitor.h>

using namespace vr;

void LightNode::accept(NodeVisitor& visitor) {
    visitor.visit(this);
}

BoundingBox LightNode::calculateBoundingBox(glm::mat4 t_mat) {
    return BoundingBox();
}
