#include <vr/Group.h>

using namespace vr;

void Group::accept(NodeVisitor& visitor) {
    std::cerr << "Accept: " << m_name << std::endl;
    visitor.visit(this);
}

void Group::addChild(std::shared_ptr<Node> node) {
    m_children.push_back(node);
}

NodeVector& Group::getChildren() {
    return m_children;
}

BoundingBox Group::calculateBoundingBox() {
    BoundingBox bbox;
    for (auto child : m_children) {
        bbox.expand(child->calculateBoundingBox());
    }
    return bbox;
}
