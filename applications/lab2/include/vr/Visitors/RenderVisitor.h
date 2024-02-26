#pragma once

#include <stack>

#include "NodeVisitor.h"

/**
 * A visitor that traverses the scene graph, collecting states and transformations.
 * When a geometry node is visited, the visitor will render the geometry using the top state and transformation of respective stacks.
 */

namespace vr {
class RenderVisitor : public NodeVisitor {
   public:
    RenderVisitor();
    void visit(Geometry* geometry) override;
    void visit(Transform* transform) override;
    void visit(Group* group) override;
    void visit(LodNode* lodNode) override;
    void visit(LightNode* lightNode) override;
    void visit(CameraNode* cameraNode) override;

   private:
    std::stack<glm::mat4> m_matrixStack;
};

}  // namespace vr
