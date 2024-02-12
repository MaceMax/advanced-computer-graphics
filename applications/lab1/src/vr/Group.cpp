#include <vr/Group.h>

using namespace vr;

void Group::accept(NodeVisitor& visitor) {
    visitor.visit(this);
}

void Group::addChild(std::shared_ptr<Node> node) {
    m_children.push_back(node);
}

NodeVector& Group::getChildren() {
    return m_children;
}

void Group::setChildren(NodeVector& children) {
    m_children = children;
}

BoundingBox Group::calculateBoundingBox(glm::mat4 t_mat) {
    BoundingBox bbox;
    for (auto child : m_children) {
        bbox.expand(child->calculateBoundingBox(t_mat));
    }
    return bbox;
}
