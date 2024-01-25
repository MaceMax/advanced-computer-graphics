#pragma once

#include <stack>

#include "vr/Geometry.h"
#include "vr/Transform.h"

namespace vr {
class NodeVisitor {
   public:
    virtual void visit(Geometry& geometry) = 0;
    virtual void visit(Transform& transform) = 0;
    void visit(Group* group);

   protected:
    StateStack m_stateStack;
};
typedef std::stack<std::shared_ptr<State>> StateStack;

}  // namespace vr