#include <vr/Nodes/Geometry.h>
#include <vr/Nodes/Group.h>
#include <vr/Nodes/LodNode.h>
#include <vr/Nodes/Transform.h>
#include <vr/Visitors/UpdateVisitor.h>

using namespace vr;

void UpdateVisitor::visit(Geometry* geometry) {
    if (geometry->hasCallbacks()) {
        for (auto& callback : geometry->getUpdateCallbacks()) {
            callback->execute(*geometry);
        }
    }
}

void UpdateVisitor::visit(Transform* transform) {
    if (transform->hasCallbacks()) {
        for (auto& callback : transform->getUpdateCallbacks()) {
            callback->execute(*transform);
        }
    }

    for (auto& child : transform->getChildren()) {
        child->accept(*this);
    }
}

void UpdateVisitor::visit(Group* group) {
    if (group->hasCallbacks()) {
        for (auto& callback : group->getUpdateCallbacks()) {
            callback->execute(*group);
        }
    }

    for (auto& child : group->getChildren()) {
        child->accept(*this);
    }
}

void UpdateVisitor::visit(LodNode* lodNode) {
    if (lodNode->hasCallbacks()) {
        for (auto& callback : lodNode->getUpdateCallbacks()) {
            callback->execute(*lodNode);
        }
    }

    Node* node = lodNode->getChild(m_activeCamera->getPosition());
    if (node) {
        node->accept(*this);
    }
}
