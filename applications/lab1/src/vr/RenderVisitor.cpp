#include <vr/RenderVisitor.h>

#include <iostream>

using namespace vr;

void RenderVisitor::visit(Geometry* geometry) {
    std::cerr << "Visit: " << geometry->getName() << std::endl;
    // Geometry's state has precedence over the parent's state

    std::shared_ptr<State> state;
    if (geometry->getState()) {
        std::cerr << "Geometry has state" << std::endl;
        state = *(m_stateStack.top()) + *(geometry->getState());
    } else {
        std::cerr << "Geometry does not have state" << std::endl;
        state = m_stateStack.top();
    }

    // glm::mat4 m_matrix = m_matrixStack.top();

    state.get()->apply();
    geometry->draw(state.get()->getShader(), glm::mat4(1.0f));
}

void RenderVisitor::visit(Transform* transform) {
    std::cerr << "Visit: " << transform->getName() << std::endl;

    // Push the current matrix onto the stack
    m_stateStack.push(*(m_stateStack.top()) + *(transform->getState()));
    // m_matrixStack.push(m_matrixStack.top() * transform->getMatrix());
    for (auto& child : transform->getChildren()) {
        child->accept(*this);
    }
    // Pop the matrix off the stack
    // m_matrixStack.pop();
    m_stateStack.pop();
}

void RenderVisitor::visit(Group* group) {
    std::cerr << "Visit: " << group->getName() << std::endl;

    if (group->getState()) {
        std::cerr << "Group has state" << std::endl;
        m_stateStack.push(group->getState());
    } else {
        std::cerr << "Group does not have state" << std::endl;
    }

    for (auto& child : group->getChildren()) {
        child->accept(*this);
    }
    m_stateStack.pop();
}
