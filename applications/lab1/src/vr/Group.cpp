#include "Group.h"

using namespace vr;

void Group::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void Group::addChild(std::shared_ptr<Node>& node) {
    m_children.push_back(node);
}

NodeVector& Group::getChildren() {
    return m_children;
}
