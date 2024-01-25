#include <vr/NodeVisitor.h>

using namespace vr;

void NodeVisitor::visit(Group* group) {
    m_stateStack.push(*(m_stateStack.top()) + *(group.getState()));
    for (auto& child : group.getChildren()) {
        child->accept(*this);
    }
    m_stateStack.pop();
}
