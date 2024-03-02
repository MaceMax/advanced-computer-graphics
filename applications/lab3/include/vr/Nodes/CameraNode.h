#pragma once
#include "vr/Nodes/Group.h"

namespace vr {
class CameraNode : public Group {
   public:
    CameraNode(std::shared_ptr<Camera> camera, const std::string& name = "CameraNode") : m_camera(camera), Group(name) {}
    virtual void accept(NodeVisitor& visitor) override;

    void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
    std::shared_ptr<Camera> getCamera() { return m_camera; }

   private:
    std::shared_ptr<Camera> m_camera;
};
}  // namespace vr