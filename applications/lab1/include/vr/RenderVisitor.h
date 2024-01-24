#include <stack>

#include "vr/NodeVisitor.h"

namespace vr {
class RenderVisitor : public NodeVisitor {
   public:
    void visit(Geometry& geometry) override;
    void visit(Transform& transform) override;

   private:
    std::stack<glm::mat4> m_matrixStack;
    StateStack m_stateStack;
};

typedef std::stack<std::shared_ptr<State>> StateStack;
}  // namespace vr
