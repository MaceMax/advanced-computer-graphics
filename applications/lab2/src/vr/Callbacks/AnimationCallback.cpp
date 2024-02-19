#include <vr/Callbacks/AnimationCallback.h>
#include <vr/Nodes/Transform.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace vr;

void AnimationCallback::execute(Transform& node) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - m_startTime).count();

    elapsedTime *= m_speed;

    if (isDone)
        return;

    // Interpolate between frames
    int nextFrame = m_currentFrame + 1;
    if (nextFrame >= m_frames.size()) {
        nextFrame = 0;
    }

    if (!m_loop && nextFrame == 0) {
        m_currentFrame = 0;
        m_startTime = currentTime;
        isDone = true;
        return;
    } else if (m_loop && nextFrame == 0) {
        m_currentFrame = 0;
        m_startTime = currentTime;
        return;
    }

    // Calculate the interpolation factor
    float t = elapsedTime / m_frames[m_currentFrame].m_duration;
    if (t > 1.0f) {
        t = 1.0f;
        m_currentFrame = nextFrame;
        m_startTime = currentTime;
    }

    // Decompose the transformation matrices into translation, rotation, and scale
    glm::vec3 translation1, translation2, scale1, scale2;
    glm::quat rotation1, rotation2;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(m_frames[m_currentFrame].m_transform, scale1, rotation1, translation1, skew, perspective);
    glm::decompose(m_frames[nextFrame].m_transform, scale2, rotation2, translation2, skew, perspective);

    // Interpolate each component separately, otherwise the rotation will not be correct (had scaling issues)
    glm::vec3 translation = glm::mix(translation1, translation2, t);
    glm::quat rotation = glm::slerp(rotation1, rotation2, t);
    glm::vec3 scale = glm::mix(scale1, scale2, t);

    // Combine the interpolated components back into a transformation matrix
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) *
                          glm::mat4_cast(rotation) *
                          glm::scale(glm::mat4(1.0f), scale);

    node.setMatrix(transform);
}

void AnimationCallback::addFrame(AnimationFrame frame) {
    m_frames.push_back(frame);
}