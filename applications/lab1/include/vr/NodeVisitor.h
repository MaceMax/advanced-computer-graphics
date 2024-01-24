#pragma once

#include "vr/Geometry.h"
#include "vr/Transform.h"

namespace vr {
class NodeVisitor {
   public:
    virtual void visit(Geometry& geometry) = 0;
    virtual void visit(Transform& transform) = 0;
    void visit(Group& group);
};

}  // namespace vr