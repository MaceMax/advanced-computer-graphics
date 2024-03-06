#include <vr/Nodes/CameraNode.h>
#include <vr/Nodes/Geometry.h>
#include <vr/Nodes/Group.h>
#include <vr/Nodes/LightNode.h>
#include <vr/Nodes/LodNode.h>
#include <vr/Nodes/Transform.h>
#include <vr/Visitors/DepthVisitor.h>

using namespace vr;

DepthVisitor::DepthVisitor() {
    m_matrixStack.push(glm::mat4(1.0f));
    m_directionalDepthShader = std::make_shared<Shader>("shaders/depth-shader.vs", "shaders/depth-shader.fs");
    m_pointDepthShader = std::make_shared<Shader>("shaders/point-depth-shader.vs", "shaders/point-depth-shader.fs", "shaders/point-depth-shader.gs");
    glGenFramebuffers(1, &fbo);
}

DepthVisitor::~DepthVisitor() {
    glDeleteFramebuffers(1, &fbo);
    m_activeLight = nullptr;
}

void DepthVisitor::setupRenderState(const std::shared_ptr<Light> light, int depthMapIndex, unsigned int textureID) {
    m_activeLight = light;

    glViewport(0, 0, DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    if (m_activeLight->getPosition().w == 0) {
        m_depthShader = m_directionalDepthShader;

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureID, 0, depthMapIndex);

    } else {
        m_depthShader = m_pointDepthShader;
        for (int face = 0; face < 6; face++) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureID, 0, depthMapIndex * 6 + face);
        }
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Depth visitor framebuffer is not complete" << std::endl;
        exit(1);
    }

    glClear(GL_DEPTH_BUFFER_BIT);
}

void DepthVisitor::visit(Geometry* geometry) {
    m_depthShader->use();

    // Depending on the light type, set the appropriate uniforms as they are use different shaders
    if (m_activeLight->getPosition().w == 0) {
        m_depthShader->setMat4("lsm", m_activeLight->getProjection() * m_activeLight->getView());
    } else {
        for (size_t i = 0; i < 6; i++) {
            m_depthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", m_activeLight->getShadowMatrix(i));
        }
        m_depthShader->setFloat("farPlane", m_activeLight->getFarPlane());
        m_depthShader->setVec3("lightPos", glm::vec3(m_activeLight->getTransform() * m_activeLight->getPosition()));
    }

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

void DepthVisitor::visit(CameraNode* cameraNode) {}