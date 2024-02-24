#include <vr/Nodes/Geometry.h>
#include <vr/Nodes/Group.h>
#include <vr/Nodes/LightNode.h>
#include <vr/Nodes/LodNode.h>
#include <vr/Nodes/Transform.h>
#include <vr/Visitors/DepthVisitor.h>

using namespace vr;

DepthVisitor::DepthVisitor() {
    m_matrixStack.push(glm::mat4(1.0f));
    m_depthShader = std::make_shared<Shader>("shaders/depth-shader.vs", "shaders/depth-shader.fs");

    // Create a framebuffer object
    glGenFramebuffers(1, &fbo);
}

DepthVisitor::~DepthVisitor() {
    glDeleteFramebuffers(1, &fbo);
}

void DepthVisitor::setupRenderState(GLuint depthTexture, const glm::mat4& lightView, const glm::mat4& lightProjection) {
    m_depthTexture = depthTexture;
    m_lightView = lightView;
    m_lightProjection = lightProjection;

    glViewport(0, 0, DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION);
    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete" << std::endl;
    }
}

void DepthVisitor::visit(Geometry* geometry) {
    m_depthShader->use();
    m_depthShader->setMat4("lsm", m_lightProjection * m_lightView);
    geometry->draw(m_depthShader, m_matrixStack.top(), true);
}

void DepthVisitor::visit(Transform* transform) {
    m_matrixStack.push(m_matrixStack.top() * transform->getMatrix());

    for (auto& child : transform->getChildren()) {
        child->accept(*this);
    }

    m_matrixStack.pop();
}

void DepthVisitor::visit(Group* group) {
    for (auto& child : group->getChildren()) {
        child->accept(*this);
    }
}

void DepthVisitor::visit(LodNode* lodNode) {
    Node* child = lodNode->getChild(m_activeCamera->getPosition());

    if (child) {
        child->accept(*this);
    }
}

void DepthVisitor::visit(LightNode* lightNode) {}