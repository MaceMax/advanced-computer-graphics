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

BoundingBox Transform::calculateBoundingBox(glm::mat4 t_mat) {
    // Apply the transform to the bounding box
    BoundingBox bbox;
    for (auto child : m_children) {
        bbox.expand(child->calculateBoundingBox(t_mat * t_matrix));
    }
    return bbox;
}
