#pragma once

#include "vr/NodeVisitor.h"

namespace vr {
class RenderVisitor : public NodeVisitor {
   public:
    void visit(Geometry& geometry) override;
    void visit(Transform& transform) override;

   private:
    std::stack<glm::mat4> m_matrixStack;
};

}  // namespace vr
