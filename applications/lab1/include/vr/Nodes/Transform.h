#pragma once

#include "Group.h"

/**
 *  A Transform node. A transform node is a node that can have children and has a transformation matrix.
 */

namespace vr {
class Transform : public Group {
   public:
    Transform(const std::string& name = "Transform") : Group(name) {}
    virtual void accept(NodeVisitor& visitor) override;

    /**
     * @brief Sets the transformation matrix of the transform node
     *
     * @param matrix The transformation matrix
     */
    void setMatrix(const glm::mat4& matrix);

    /**
     * @brief Gets the transformation matrix of the transform node
     *
     * @return glm::mat4 The transformation matrix
     */
    glm::mat4 getMatrix();
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) override;

   private:
    glm::mat4 t_matrix;
};
}  // namespace vr