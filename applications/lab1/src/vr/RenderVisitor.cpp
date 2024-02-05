#include <vr/RenderVisitor.h>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace vr;

void RenderVisitor::visit(Geometry* geometry) {
    std::shared_ptr<State> state = nullptr;
    if (geometry->getState()) {
        state = *(m_stateStack.top()) + *(geometry->getState());
    } else {
        state = m_stateStack.top();
    }

    state->apply();

    geometry->draw(state->getShader(), m_matrixStack.empty() ? glm::mat4(1.0f) : m_matrixStack.top());
}

void RenderVisitor::visit(Transform* transform) {
    if (m_matrixStack.empty()) {
        m_matrixStack.push(transform->getMatrix());
    } else {
        m_matrixStack.push(m_matrixStack.top() * transform->getMatrix());
    }

    // Push the current matrix onto the stack
    if (transform->getState()) {
        m_stateStack.push(*(m_stateStack.top()) + *(transform->getState()));
    }

    for (auto& child : transform->getChildren()) {
        child->accept(*this);
    }

    m_matrixStack.pop();
    if (transform->getState()) {
        m_stateStack.pop();
    }
}

void RenderVisitor::visit(Group* group) {
    if (group->isRoot() && group->getState()) {
        m_stateStack.push(group->getState());
    } else if (group->getState()) {
        m_stateStack.push(*(m_stateStack.top()) + *(group->getState()));
    }

    for (auto& child : group->getChildren()) {
        child->accept(*this);
    }

    if (group->getState()) {
        m_stateStack.pop();
    }
}
