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
    void setupRenderState(const std::shared_ptr<Light> light);

   private:
    std::stack<glm::mat4> m_matrixStack;
    std::shared_ptr<Light> m_activeLight;
    GLuint fbo;

    std::shared_ptr<Shader> m_directionalDepthShader;
    std::shared_ptr<Shader> m_pointDepthShader;
    std::shared_ptr<Shader> m_depthShader;
};

}  // namespace vr