#include <vr/RenderVisitor.h>

using namespace vr;

void RenderVisitor::visit(Geometry& geometry) {
    // Geometry's state has precedence over the parent's state
    std::shared_ptr<State> state = *(m_stateStack.top()) + *(geometry.getState());
    glm::mat4 m_matrix = m_matrixStack.top();

    state.get()->apply();
    geometry.draw(state.get()->getShader(), m_matrix);
}

void RenderVisitor::visit(Transform& transform) {
    // Push the current matrix onto the stack
    m_stateStack.push(*(m_stateStack.top()) + *(transform.getState()));
    m_matrixStack.push(m_matrixStack.top() * transform.getMatrix());
    for (auto& child : transform.getChildren()) {
        child->accept(*this);
    }
    // Pop the matrix off the stack
    m_matrixStack.pop();
    m_stateStack.pop();
}
