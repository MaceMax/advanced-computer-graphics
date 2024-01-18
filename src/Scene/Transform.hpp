#include <glm/glm.hpp>

#include "Group.hpp"

class Transform : public Group {
   public:
    Transform() = default;
    ~Transform() = default;

    void Accept(NodeVisitor& nodeVisitor) override;

   private:
    glm::mat4 transformMatrix;
};