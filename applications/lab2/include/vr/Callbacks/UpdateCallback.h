#pragma once

#include <memory>
#include <vector>

namespace vr {
class Geometry;
class Group;
class Transform;
class LodNode;
class LightNode;
class CameraNode;

class UpdateCallback {
   public:
    virtual void execute(Geometry& node) = 0;
    virtual void execute(Group& node) = 0;
    virtual void execute(Transform& node) = 0;
    virtual void execute(LodNode& node) = 0;
    virtual void execute(LightNode& node) = 0;
    virtual void execute(CameraNode& node) = 0;
};
typedef std::vector<std::shared_ptr<UpdateCallback>> UpdateCallbackVector;
}  // namespace vr