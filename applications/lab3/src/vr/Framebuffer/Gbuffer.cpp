#include <vr/Frambuffer/Gbuffer.h>

#include <iostream>

using namespace vr;

Gbuffer::Gbuffer(unsigned int width, unsigned int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    m_position = std::make_shared<Texture>();
    m_position->createFramebufferTexture(G_BUFFER_POSITION_SLOT, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_position->id(), 0);

    m_normal = std::make_shared<Texture>();
    m_normal->createFramebufferTexture(G_BUFFER_NORMAL_SLOT, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal->id(), 0);

    m_albedo = std::make_shared<Texture>();
    m_albedo->createFramebufferTexture(G_BUFFER_ALBEDO_SLOT, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedo->id(), 0);

    m_metallicRoughness = std::make_shared<Texture>();
    m_metallicRoughness->createFramebufferTexture(G_BUFFER_METALLIC_ROUGHNESS, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_metallicRoughness->id(), 0);

    GLuint attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    m_depth = std::make_shared<Texture>();
    m_depth->createFramebufferTexture(G_BUFFER_DEPTH_SLOT, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->id(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Gbuffer framebuffer is not complete" << std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Gbuffer::rescaleFramebuffer(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    m_position->rescale(width, height);
    m_normal->rescale(width, height);
    m_albedo->rescale(width, height);
    m_metallicRoughness->rescale(width, height);
    m_depth->rescale(width, height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_position->id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal->id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedo->id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_metallicRoughness->id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->id(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Gbuffer::bindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Gbuffer::unbindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Gbuffer::bindTextures() {
    m_position->bind();
    m_normal->bind();
    m_albedo->bind();
    m_metallicRoughness->bind();
}

void Gbuffer::unbindTextures() {
    m_position->unbind();
    m_normal->unbind();
    m_albedo->unbind();
    m_metallicRoughness->unbind();
}