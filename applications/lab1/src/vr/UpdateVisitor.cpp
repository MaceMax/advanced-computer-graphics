#include <vr/Geometry.h>
#include <vr/Group.h>
#include <vr/Transform.h>
#include <vr/UpdateVisitor.h>

using namespace vr;

void UpdateVisitor::visit(Geometry* geometry) {
    if (geometry->hasCallback()) {
        geometry->getUpdateCallback()->execute(*geometry);
    }
}

void UpdateVisitor::visit(Transform* transform) {
    if (transform->hasCallback()) {
        transform->getUpdateCallback()->execute(*transform);
    }

    for (auto& child : transform->getChildren()) {
        child->accept(*this);
    }
}

void UpdateVisitor::visit(Group* group) {
    if (group->hasCallback()) {
        group->getUpdateCallback()->execute(*group);
    }

    for (auto& child : group->getChildren()) {
        child->accept(*this);
    }
}
