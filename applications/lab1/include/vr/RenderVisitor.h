#pragma once

#include <stack>

#include "vr/Geometry.h"
#include "vr/Group.h"
#include "vr/NodeVisitor.h"
#include "vr/Transform.h"

namespace vr {
class RenderVisitor : public NodeVisitor {
   public:
    void visit(Geometry* geometry) override;
    void visit(Transform* transform) override;
    void visit(Group* group) override;
    int getSteps() { return steps; }

   private:
    std::stack<glm::mat4> m_matrixStack;
    int steps = 0;
};

}  // namespace vr
