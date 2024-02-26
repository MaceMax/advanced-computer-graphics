#include <vr/Callbacks/UpdateCallback.h>
#include <vr/Nodes/CameraNode.h>
#include <vr/Nodes/Geometry.h>
#include <vr/Nodes/Group.h>
#include <vr/Nodes/LightNode.h>
#include <vr/Nodes/LodNode.h>
#include <vr/Nodes/Transform.h>
#include <vr/Visitors/RenderVisitor.h>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace vr;

RenderVisitor::RenderVisitor() {
    m_matrixStack.push(glm::mat4(1.0f));
}

void RenderVisitor::visit(Geometry* geometry) {
    std::shared_ptr<State> state = nullptr;

    // Geometry always has a state
    state = *(m_stateStack.top()) + *(geometry->getState());

    state->apply();
    state->getShader()->setVec3("viewPos", m_activeCamera->getPosition());
    m_activeCamera->apply(state->getShader());
    geometry->draw(state->getShader(), m_matrixStack.top());
}

void RenderVisitor::visit(Transform* transform) {
    m_matrixStack.push(m_matrixStack.top() * transform->getMatrix());

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
    if (m_stateStack.empty() && group->hasState()) {
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

    Node* child = lodNode->getChild(m_activeCamera->getPosition());

    if (child) {
        child->accept(*this);
    }

    if (lodNode->hasState()) {
        m_stateStack.pop();
    }
}

void RenderVisitor::visit(LightNode* lightNode) {
    glm::mat4 t_mat = m_matrixStack.top();
    lightNode->getLight()->setTransform(t_mat);
}

void RenderVisitor::visit(CameraNode* cameraNode) {
    glm::mat4 t_mat = m_matrixStack.top();
    cameraNode->getCamera()->setTransform(t_mat);
}