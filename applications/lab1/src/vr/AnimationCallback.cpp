#include <vr/AnimationCallback.h>
#include <vr/Geometry.h>
#include <vr/Group.h>
#include <vr/Transform.h>

using namespace vr;

void AnimationCallback::execute(Transform& node) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - m_startTime).count();

    node.setMatrix(animation->getTransformation(elapsedTime));
}