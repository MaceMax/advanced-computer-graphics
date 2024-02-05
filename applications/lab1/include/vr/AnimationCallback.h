#pragma once

#include <chrono>

#include "UpdateCallback.h"
#include "vr/Animation/Animation.h"

namespace vr {

class AnimationCallback : public UpdateCallback {
   public:
    AnimationCallback(Animation* animation) : animation(animation), m_startTime(std::chrono::high_resolution_clock::now()) {}
    void execute(Geometry& node) override {}
    void execute(Group& node) override {}
    void execute(Transform& node) override;

   private:
    Animation* animation;
    std::chrono::high_resolution_clock::time_point m_startTime;
};

}  // namespace vr
