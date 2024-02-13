#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <memory>

#include "vr/BoundingBox.h"
#include "vr/Callbacks/UpdateCallback.h"
#include "vr/State/State.h"

namespace vr {
/**
 * Simple class representing a node in a scene graph
 */
class NodeVisitor;

class Node {
   public:
    Node(const std::string& name = "Node") : m_name(name) {}

    /**
     * @brief Virtual accept method for the visitor pattern
     *
     * @param visitor The visitor to accept
     */
    virtual void accept(NodeVisitor& visitor) = 0;

    /**
     * @brief Virtual method to calculate the bounding box of the node
     *
     * @param t_mat The transformation matrix
     * @return BoundingBox The bounding box of the node
     */
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) = 0;

    /**
     * @brief Returns the name of the node
     *
     * @return std::string The name of the node
     */
    std::string getName() { return m_name; }

    /**
     * @brief Checks if the node has a state
     *
     * @param bool True if the node has a state, false otherwise
     */
    bool hasState() { return m_state != nullptr; }

    /**
     * @brief Checks if the node has update callbacks
     *
     * @param bool True if the node has update callbacks, false otherwise
     */
    bool hasCallbacks() { return !m_updateCallbacks.empty(); }

    /**
     * @brief Adds an update callback to the node
     *
     * @param callback The update callback to add
     */
    void addUpdateCallback(std::shared_ptr<UpdateCallback> callback) { m_updateCallbacks.push_back(callback); }

    /**
     * @brief Gets the update callbacks of the node
     *
     * @return UpdateCallbackVector The update callbacks of the node
     */
    UpdateCallbackVector getUpdateCallbacks() { return m_updateCallbacks; }

    /**
     * @brief Sets the state of the node
     *
     * @param state The state to set
     */
    void setState(std::shared_ptr<State> state) { m_state = state; }

    /**
     * @brief Gets the state of the node
     *
     * @return std::shared_ptr<State> The state of the node
     */
    std::shared_ptr<State>& getState() { return m_state; }

   protected:
    UpdateCallbackVector m_updateCallbacks;
    std::string m_name;
    std::shared_ptr<State> m_state;
};

typedef std::vector<std::shared_ptr<Node>> NodeVector;

}  // namespace vr
