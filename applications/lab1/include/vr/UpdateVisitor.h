#pragma once

#include "vr/NodeVisitor.h"
#include "vr/UpdateCallback.h"

namespace vr {
class UpdateVisitor : public NodeVisitor {
   public:
    void visit(Geometry* geometry) override;
    void visit(Transform* transform) override;
    void visit(Group* group) override;
};
}  // namespace vr