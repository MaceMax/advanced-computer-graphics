#include "Node.h"

namespace vr {
class Group : public Node {
   public:
    Group(const std::string& name = "Group", std::shared_ptr<Node> parent = nullptr) : Node(name, parent) {}
    virtual void accept(NodeVisitor& visitor) override;
    void addChild(std::shared_ptr<Node>& node);

    NodeVector& getChildren();

   private:
    NodeVector m_children;
};
typedef std::vector<std::shared_ptr<Node>> NodeVector;

}  // namespace vr