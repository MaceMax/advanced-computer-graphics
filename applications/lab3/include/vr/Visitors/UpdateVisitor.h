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
    void visit(LightNode* lightNode) override;
    void visit(CameraNode* cameraNode) override;
    bool sceneChanged() const { return m_sceneChanged; }
    void setSceneChanged(bool changed) { m_sceneChanged = changed; }

   private:
    bool m_sceneChanged;
};
}  // namespace vr