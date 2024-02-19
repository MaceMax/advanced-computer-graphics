#pragma once

#include "NodeVisitor.h"
#include "vr/Callbacks/UpdateCallback.h"

/**
 * A visitor that traverses the scene graph, calling the update callbacks of the nodes.
 */

namespace vr {
class UpdateVisitor : public NodeVisitor {
   public:
    void visit(Geometry* geometry) override;
    void visit(Transform* transform) override;
    void visit(Group* group) override;
    void visit(LodNode* lodNode) override;
};
}  // namespace vr