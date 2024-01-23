#include "Group.h"

namespace vr {
class Transform : public Group {
   public:
    Transform(const std::string& name = "Transform", std::shared_ptr<Node> parent = nullptr) : Group(name, parent) {}
    virtual void accept(NodeVisitor& visitor) override;
    void setMatrix(const glm::mat4& matrix);
    glm::mat4 getMatrix();

   private:
    glm::mat4 t_matrix;
};
}