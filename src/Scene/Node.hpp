
/**
 * Abstract class for all nodes in the scene graph. Accepts a visitor.
 */
class Node {
   public:
    Node() = default;
    virtual ~Node() = default;

    virtual void Accept(NodeVisitor const&) = 0;
};