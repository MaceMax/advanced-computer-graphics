#pragma once

#include <chrono>
#include <glm/glm.hpp>

#include "UpdateCallback.h"

namespace vr {

struct AnimationFrame {
    glm::mat4 m_transform;
    float m_duration;
};

class AnimationCallback : public UpdateCallback {
   public:
    AnimationCallback(float speed, bool loop) : m_speed(speed), m_loop(loop), m_startTime(std::chrono::high_resolution_clock::now()) {}
    void execute(Geometry& node) override {}
    void execute(Group& node) override {}
    void execute(Transform& node) override;
    void execute(LodNode& node) override {}
    void execute(LightNode& node) override {}
    void execute(CameraNode& node) override {}
    void addFrame(AnimationFrame frame);

   private:
    std::chrono::high_resolution_clock::time_point m_startTime;
    std::vector<AnimationFrame> m_frames;
    glm::mat4 m_initialTransform;
    int m_currentFrame = 0;
    float m_speed;
    bool firstLoop = true;
    bool m_loop = false;
    bool isDone = false;
};

}  // namespace vr
