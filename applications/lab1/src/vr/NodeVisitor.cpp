#include <vr/NodeVisitor.h>

using namespace vr;

void NodeVisitor::visit(Group& group) {
    for (auto& child : group.getChildren()) {
        child->accept(*this);
    }
}
