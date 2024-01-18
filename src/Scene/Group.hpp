#include <memory>
#include <vector>

#include "Node.hpp"

class Group : public Node {
   public:
    Group() = default;
    ~Group() = default;

    void Accept(NodeVisitor& nodeVisitor) override;
    void AddChild(std::unique_ptr<Node> child);

   protected:
    std::vector<std::unique_ptr<Node>> children;
};