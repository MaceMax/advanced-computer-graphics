#pragma once

#include <vr/BoundingBox.h>
#include <vr/NodeVisitor.h>
#include <vr/State.h>

#include <glm/glm.hpp>
#include <memory>

namespace vr {
/**
Simple class that store a number of meshes and draws it
*/
class Node {
   public:
    Node(const std::string& name = "Node") : m_name(name) {}
    virtual void accept(NodeVisitor& visitor) = 0;
    std::string getName() { return m_name; }
    bool hasState() { return m_state != nullptr; }
    void setState(std::shared_ptr<State> state) { m_state = state; }
    std::shared_ptr<State> getState() { return m_state; }

    /// Calculate and return a bounding box for this Node based on its Mesh objects
    BoundingBox calculateBoundingBox();

   protected:
    /**
     /Name of the node
     */

    std::string m_name;
    std::shared_ptr<State> m_state;
};

}  // namespace vr
