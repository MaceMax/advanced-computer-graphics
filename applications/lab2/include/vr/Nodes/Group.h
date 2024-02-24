#pragma once
#include "Node.h"
#include "vr/Visitors/NodeVisitor.h"

namespace vr {

/**
 *  A Group node. A group node is a node that can have children.
 */

class Group : public Node {
   public:
    /**
     * @brief Construct a new Group object
     *
     * @param name
     */
    Group(const std::string& name = "Group", bool excludeFromBoundingBox = false) : m_excludeFromBoundingBox(excludeFromBoundingBox), Node(name) {}

    virtual void accept(NodeVisitor& visitor) override;

    /**
     * @brief Add a child node to the group
     *
     * @param node The child node to add
     */
    void addChild(std::shared_ptr<Node> node);

    /**
     * @brief Calculate the bounding box of the group
     *
     * @param t_mat The transformation matrix
     * @return BoundingBox The bounding box of the group
     */
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) override;

    /**
     * @brief Get the children of the group
     *
     * @return NodeVector The children of the group
     */
    NodeVector& getChildren();

    /**
     * @brief Set the children of the group
     *
     * @param children The children of the group
     */
    void setChildren(NodeVector& children);

   protected:
    NodeVector m_children;
    bool m_excludeFromBoundingBox;
};

}  // namespace vr