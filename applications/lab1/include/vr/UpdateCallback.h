#pragma once

namespace vr {
class Geometry;
class Group;
class Transform;

class UpdateCallback {
   public:
    virtual void execute(Geometry& node) = 0;
    virtual void execute(Group& node) = 0;
    virtual void execute(Transform& node) = 0;
};
}  // namespace vr