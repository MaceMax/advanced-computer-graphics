#pragma once

#include <stack>

#include "vr/Scene/Camera.h"
#include "vr/State/State.h"

/**
 * An abstract class for visiting nodes in the scene graph
 */

namespace vr {
class Geometry;
class Transform;
class Group;
class LodNode;
class LightNode;

typedef std::stack<std::shared_ptr<State>> StateStack;
class NodeVisitor {
   public:
    virtual void visit(Geometry* geometry) = 0;
    virtual void visit(Transform* transform) = 0;
    virtual void visit(Group* group) = 0;
    virtual void visit(LodNode* lodNode) = 0;
    virtual void visit(LightNode* lightNode) = 0;
    void setActiveCamera(std::shared_ptr<Camera> camera) { m_activeCamera = camera; }

   protected:
    StateStack m_stateStack;
    std::shared_ptr<Camera> m_activeCamera;
};

}  // namespace vr