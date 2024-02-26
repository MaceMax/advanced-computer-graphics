#pragma once
#include "vr/Nodes/Group.h"

namespace vr {
class CameraNode : public Group {
   public:
    CameraNode(const std::string& name = "CameraNode") : Group(name) {}
    virtual void accept(NodeVisitor& visitor) override;

   private:
    std::shared_ptr<Camera> m_camera;
};
}  // namespace vr