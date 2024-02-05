#pragma once

#include "Animation.h"

namespace vr {
class CircleAnimation : public Animation {
   public:
    CircleAnimation(float radius, float speed) : radius(radius), speed(speed) {}
    glm::mat4 getTransformation(float time) override;

   private:
    float radius;
    float speed;
};
}  // namespace vr