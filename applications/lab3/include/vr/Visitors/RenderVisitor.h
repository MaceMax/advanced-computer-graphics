#pragma once

#include <stack>

#include "NodeVisitor.h"

/**
 * A visitor that traverses the scene graph, collecting states and transformations.
 * When a geometry node is visited, the visitor will render the geometry using the top state and transformation of respective stacks.
 */

namespace vr {
class RenderVisitor : public NodeVisitor {
   public:
    RenderVisitor(int width, int height);
    void visit(Geometry* geometry) override;
    void visit(Transform* transform) override;
    void visit(Group* group) override;
    void visit(LodNode* lodNode) override;
    void visit(LightNode* lightNode) override;
    void visit(CameraNode* cameraNode) override;
    std::shared_ptr<Texture> getTexture() { return texture; }
    void rescaleFramebuffer(int width, int height);

    void bindFBO();
    void unbindFBO();

   private:
    std::stack<glm::mat4> m_matrixStack;
    GLuint fbo;
    GLuint rbo;
    std::shared_ptr<Texture> texture;
};

}  // namespace vr
