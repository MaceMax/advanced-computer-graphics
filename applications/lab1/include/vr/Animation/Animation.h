#pragma once

#include <glm/glm.hpp>

/**
 * An abstract class that represents an animation.
 */
namespace vr {
class Animation {
   public:
    virtual glm::mat4 getTransformation(float time) = 0;
};

}  // namespace vr