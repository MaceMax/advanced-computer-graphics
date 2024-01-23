#pragma once

#include <vr/Shader.h>

#include <glm/glm.hpp>
#include <memory>

#include "BoundingBox.h"
#include "Material.h"
#include "Mesh.h"
#include "NodeVisitor.h"

namespace vr {
/**
Simple class that store a number of meshes and draws it
*/
class Node {
   public:
    Node(const std::string& name = "Node", std::shared_ptr<Node> parent = nullptr) : m_name(name), m_parent(parent) {}
    virtual void accept(NodeVisitor& visitor) = 0;
    std::string getName() { return m_name; }
    std::shared_ptr<Node> getParent() { return m_parent; }

    /// Calculate and return a bounding box for this Node based on its Mesh objects
    BoundingBox calculateBoundingBox();

   protected:
    /**
     /Name of the node
     */
    std::shared_ptr<Node> m_parent;
    std::string m_name;
};

}  // namespace vr
