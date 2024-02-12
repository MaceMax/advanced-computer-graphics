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
    Node(const std::string& name = "Node") : m_name(name), m_updateCallback(nullptr) {}
    virtual void accept(NodeVisitor& visitor) = 0;
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) = 0;
    std::string getName() { return m_name; }
    bool hasState() { return m_state != nullptr; }
    bool hasCallback() { return m_updateCallback != nullptr; }
    void setUpdateCallback(UpdateCallback* callback) { m_updateCallback = callback; }
    UpdateCallback* getUpdateCallback() { return m_updateCallback; }
    void setState(std::shared_ptr<State> state) { m_state = state; }
    std::shared_ptr<State>& getState() { return m_state; }

   protected:
    UpdateCallback* m_updateCallback;
    std::string m_name;
    std::shared_ptr<State> m_state;
};

typedef std::vector<std::shared_ptr<Node>> NodeVector;

}  // namespace vr
