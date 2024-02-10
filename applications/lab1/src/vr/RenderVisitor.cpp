#include <vr/Geometry.h>
#include <vr/Group.h>
#include <vr/LodNode.h>
#include <vr/RenderVisitor.h>
#include <vr/Transform.h>
#include <vr/UpdateCallback.h>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace vr;

void RenderVisitor::visit(Geometry* geometry) {
    std::shared_ptr<State> state = nullptr;

    // Geometry always has a state
    state = *(m_stateStack.top()) + *(geometry->getState());

    // If the geometry group has a material, override the geometry's default material
    // Not correct. Something else must be done here.
    if (m_stateStack.top()->getMaterial() != nullptr) {
        state->setMaterial(m_stateStack.top()->getMaterial());
    }

    state->apply();
    m_activeCamera->apply(state->getShader());
    geometry->draw(state->getShader(), m_matrixStack.empty() ? glm::mat4(1.0f) : m_matrixStack.top());
}

void RenderVisitor::visit(Transform* transform) {
    if (m_matrixStack.empty()) {
        m_matrixStack.push(transform->getMatrix());
    } else {
        m_matrixStack.push(m_matrixStack.top() * transform->getMatrix());
    }

    // Push the current matrix onto the stack
    if (transform->hasState()) {
        m_stateStack.push(*(m_stateStack.top()) + *(transform->getState()));
    }

    for (auto& child : transform->getChildren()) {
        child->accept(*this);
    }

    m_matrixStack.pop();
    if (transform->hasState()) {
        m_stateStack.pop();
    }
}

void RenderVisitor::visit(Group* group) {
    if (group->isRoot()) {
        m_stateStack.push(group->getState());
    } else if (group->hasState()) {
        m_stateStack.push(*(m_stateStack.top()) + *(group->getState()));
    }

    for (auto& child : group->getChildren()) {
        child->accept(*this);
    }

    if (group->hasState()) {
        m_stateStack.pop();
    }
}

void RenderVisitor::visit(LodNode* lodNode) {
    if (lodNode->hasState()) {
        m_stateStack.push(*(m_stateStack.top()) + *(lodNode->getState()));
    }

    lodNode->applyTransformation(m_matrixStack.empty() ? glm::mat4(1.0f) : m_matrixStack.top());
    lodNode->getChild(m_activeCamera->getPosition()).accept(*this);

    if (lodNode->hasState()) {
        m_stateStack.pop();
    }
}