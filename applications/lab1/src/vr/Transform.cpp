#include <vr/Transform.h>

using namespace vr;

void Transform::accept(NodeVisitor& visitor) {
    visitor.visit(this);
}

void Transform::setMatrix(const glm::mat4& matrix) {
    t_matrix = matrix;
}

glm::mat4 Transform::getMatrix() {
    return t_matrix;
}

BoundingBox Transform::calculateBoundingBox() {
    return Group::calculateBoundingBox();
}
