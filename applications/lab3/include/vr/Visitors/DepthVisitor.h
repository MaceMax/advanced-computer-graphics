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
    void visit(CameraNode* cameraNode) override;

    /**
     * @brief Is called before the traversal of the scene graph begins.
     * This is done to set up the render state for the light, i.e bind
     * the appropriate depth shader and texture.
     *
     * @param light The light to set up the render state for
     * @param depthMapIndex Index of light in its respective depth map array
     * @param textureUnit The texture unit to bind the depth map to
     */
    void setupRenderState(const std::shared_ptr<Light> light, int depthMapIndex, unsigned int textureUnit);

   private:
    std::stack<glm::mat4> m_matrixStack;
    std::shared_ptr<Light> m_activeLight;
    GLuint fbo;

    std::shared_ptr<Shader> m_directionalDepthShader;
    std::shared_ptr<Shader> m_pointDepthShader;
    std::shared_ptr<Shader> m_depthShader;
};

}  // namespace vr