#include <vr/Animation/CircleAnimation.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace vr;

glm::mat4 CircleAnimation::getTransformation(float time) {
    float angle = time * speed;
    float x = radius * cos(angle);
    float z = radius * sin(angle);
    return glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
}