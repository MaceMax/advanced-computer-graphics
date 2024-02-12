#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <memory>

#include "vr/BoundingBox.h"
#include "vr/State.h"
#include "vr/UpdateCallback.h"

namespace vr {

class NodeVisitor;
/**
Simple class representing a node in the scene graph
*/
class Node {
   public:
    Node(const std::string& name = "Node") : m_name(name) {}
    virtual void accept(NodeVisitor& visitor) = 0;
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) = 0;
    std::string getName() { return m_name; }
    bool hasState() { return m_state != nullptr; }
    bool hasCallbacks() { return !m_updateCallbacks.empty(); }
    void addUpdateCallback(std::shared_ptr<UpdateCallback> callback) { m_updateCallbacks.push_back(callback); }
    UpdateCallbackVector getUpdateCallbacks() { return m_updateCallbacks; }
    void setState(std::shared_ptr<State> state) { m_state = state; }
    std::shared_ptr<State>& getState() { return m_state; }

   protected:
    UpdateCallbackVector m_updateCallbacks;
    std::string m_name;
    std::shared_ptr<State> m_state;
};

typedef std::vector<std::shared_ptr<Node>> NodeVector;

}  // namespace vr
