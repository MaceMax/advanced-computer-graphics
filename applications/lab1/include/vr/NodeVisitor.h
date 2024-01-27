#pragma once

#include <stack>

#include "vr/State.h"

namespace vr {
class Geometry;
class Transform;
class Group;

typedef std::stack<std::shared_ptr<State>> StateStack;
class NodeVisitor {
   public:
    virtual void visit(Geometry* geometry) = 0;
    virtual void visit(Transform* transform) = 0;
    virtual void visit(Group* group) = 0;

   protected:
    StateStack m_stateStack;
};

}  // namespace vr