#pragma once

#include "NodeVisitor.h"

/**
 * A visitor that traverses the scene graph, rendering the geometry nodes
 *  to a depth buffer from the perspective of a given light source. Does not apply any states.
 */

namespace vr {

class DepthVisitor : public NodeVisitor {
   public:
    DepthVisitor();
    ~DepthVisitor();
    void visit(Geometry* geometry) override;
    void visit(Transform* transform) override;
    void visit(Group* group) override;
    void visit(LodNode* lodNode) override;
    void visit(LightNode* lightNode) override;
    void setupRenderState(GLuint depthTexture, const glm::mat4& lightView, const glm::mat4& lightProjection);

   private:
    std::stack<glm::mat4> m_matrixStack;
    GLuint m_depthTexture;
    GLuint fbo;
    glm::mat4 m_lightView;
    glm::mat4 m_lightProjection;
    std::shared_ptr<Shader> m_depthShader;
};

}  // namespace vr