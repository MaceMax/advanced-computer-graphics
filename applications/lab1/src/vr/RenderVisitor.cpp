#include <vr/RenderVisitor.h>

using namespace vr;

void RenderVisitor::visit(Geometry& geometry) {
}

void RenderVisitor::visit(Transform& transform) {
    // Push the current matrix onto the stack
    m_matrixStack.push(m_matrixStack.top() * transform.getMatrix());
    for (auto& child : transform.getChildren()) {
        child->accept(*this);
    }
    // Pop the matrix off the stack
    m_matrixStack.pop();
}