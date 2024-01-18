#include "Group.hpp"
#include "Transform.hpp"

class NodeVisitor {
   public:
    NodeVisitor() = default;
    virtual ~NodeVisitor() = default;

    virtual void Visit(Group const&) = 0;
    virtual void Visit(Transform const&) = 0;
};