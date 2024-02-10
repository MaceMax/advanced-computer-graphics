#include <vr/Group.h>
#include <vr/LodNode.h>

#include <algorithm>

using namespace vr;

void LodNode::accept(NodeVisitor& visitor) {
    visitor.visit(this);
}

void LodNode::addChild(float distance, std::shared_ptr<Group> node) {
    m_children.push_back(std::make_pair(distance, node));
    // Keep the children sorted by distance, smallest first
    std::sort(m_children.begin(), m_children.end(), [](const GroupPair& a, const GroupPair& b) { return a.first < b.first; });
}

Group& LodNode::getChild(const glm::vec3& cameraPosition) {
    float distance = glm::distance(cameraPosition, bbox.getCenter());

    // If a max distance is set, use it
    if (m_maxDistance != -1.0f) {
        int intervalSize = m_maxDistance / m_children.size();
        int index = distance / intervalSize;

        if (index < m_children.size()) {
            return *m_children[index].second;
        } else {
            return *m_children.back().second;
        }

    } else {
        for (auto& child : m_children) {
            if (distance < child.first) {
                return *child.second;
            }
        }
    }

    return *m_children.back().second;
}

void LodNode::setMaxDistance(float maxDistance) {
    m_maxDistance = maxDistance;
}

void LodNode::applyTransformation(const glm::mat4& matrix) {
}

BoundingBox LodNode::calculateBoundingBox(glm::mat4 m_mat) {
    // Calculate the bounding box for the child with the smallest distance
    bbox = m_children.front().second->calculateBoundingBox(m_mat);
    return bbox;
}