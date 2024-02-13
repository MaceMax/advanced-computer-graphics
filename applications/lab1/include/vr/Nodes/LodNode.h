#pragma once
#include "Node.h"

namespace vr {

/**
 * A level of detail node. A level of detail node is a node that can have children with different levels of detail.
 */

typedef std::pair<float, std::shared_ptr<Group>> GroupPair;
class LodNode : public Node {
   public:
    /**
     * @brief Constructs a new Lod Node
     *
     * @param name The name of the node
     */
    LodNode(const std::string& name = "LodNode") : m_maxDistance(-1), Node(name) {}
    virtual void accept(NodeVisitor& visitor) override;

    /**
     * @brief Calculate the bounding box of the group. The bounding box is calculated by
     *        calculating the bounding box of the geometry with the highest level of detail
     *
     * @param t_mat The transformation matrix
     * @return BoundingBox The bounding box of the geometry with the highest level of detail
     */
    virtual BoundingBox calculateBoundingBox(glm::mat4 m_mat) override;

    /**
     * @brief Add a child node to the group
     *
     * @param distance The distance at which the child node should be used
     * @param node The child node to add
     */
    void addChild(float distance, std::shared_ptr<Group> node);

    /**
     * @brief Get the child node that should be used at the given distance
     *
     * @param cameraPosition The position of the camera
     * @return Group The child node that should be used
     */
    Group* getChild(const glm::vec3& cameraPosition);

    /**
     * @brief Set the maximum distance. If this is set, intervals will be calculated based on the maximum distance.
     *
     * @param maxDistance The maximum distance at which nothing will be rendered
     */
    void setMaxDistance(float maxDistance);

   private:
    std::vector<GroupPair>
        m_children;
    float m_maxDistance;
    BoundingBox bbox;
};

}  // namespace vr