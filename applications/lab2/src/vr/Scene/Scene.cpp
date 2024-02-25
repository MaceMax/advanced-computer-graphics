#include <vr/Nodes/Group.h>
#include <vr/Scene/Scene.h>
#include <vr/glErrorUtil.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace vr;

std::shared_ptr<Scene> Scene::instance = nullptr;

Scene::Scene() {
    m_camera = std::make_shared<Camera>();
    m_renderVisitor = std::make_shared<RenderVisitor>();
    m_renderVisitor->setActiveCamera(m_camera);
    m_updateVisitor = std::make_shared<UpdateVisitor>();
    m_updateVisitor->setActiveCamera(m_camera);
    m_depthVisitor = std::make_shared<DepthVisitor>();
    m_depthVisitor->setActiveCamera(m_camera);
}

std::shared_ptr<Scene> Scene::getInstance() {
    if (instance == nullptr) {
        instance = std::shared_ptr<Scene>(new Scene());
    }
    return instance;
}

bool Scene::initShaders(const std::string& vshader_filename, const std::string& fshader_filename) {
    m_shader = std::make_shared<vr::Shader>(vshader_filename, fshader_filename);
    if (!m_shader->valid())
        return false;

    m_root = std::shared_ptr<Group>(new Group("root"));

    // Create a default state. Can be overridden by the user in the scene file.
    std::shared_ptr<State> state = std::make_shared<State>(m_shader);
    state->setMaterial(std::make_shared<Material>());
    state->setLightingEnabled(true);
    state->setShadowEnabled(true);
    m_root->setState(state);

    return true;
}

void Scene::add(std::shared_ptr<Light> light) {
    m_lights.push_back(light);
    m_root->getState()->addLight(light);
}

const LightVector Scene::getLights() {
    return m_lights;
}

void Scene::setLights(LightVector lights) {
    m_lights = lights;
    m_root->getState()->setLights(m_lights);
}

std::shared_ptr<Camera> Scene::getCamera() {
    return m_camera;
}

void Scene::setGroundPlane(std::shared_ptr<Group> ground) {
    m_groundPlane = ground;
}

void Scene::cleanup() {
    if (m_shader) {
        m_shader = nullptr;
    }

    if (m_camera) {
        m_camera = nullptr;
    }

    if (m_root) {
        m_root = nullptr;
    }

    if (m_depthVisitor) {
        m_depthVisitor = nullptr;
    }

    if (m_lights.size() > 0) {
        m_lights.clear();
    }

    if (m_groundPlane) {
        m_groundPlane = nullptr;
    }
}

void Scene::useProgram() {
    m_shader->use();
}

std::shared_ptr<Group>& Scene::getRoot() {
    return m_root;
}

void Scene::toggleShadows() {
    m_shadowsEnabled = !m_shadowsEnabled;
    m_root->getState()->setShadowEnabled(m_shadowsEnabled);
}

std::shared_ptr<Light> Scene::getSelectedLight() {
    return m_lights[selectedLight];
}

BoundingBox Scene::calculateSceneBoundingBox(bool excludeGround) {
    BoundingBox box;

    if (m_groundPlane != nullptr)
        m_groundPlane->setExclude(excludeGround);

    box.expand(m_root->calculateBoundingBox(glm::mat4(1.0f)));

    // Should be excluded by default
    if (m_groundPlane != nullptr)
        m_groundPlane->setExclude(true);

    return box;
}

void Scene::render() {
    m_updateVisitor->visit(m_root.get());

    if (m_shadowsEnabled)
        renderDepthMaps();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_camera->getScreenSize().x, m_camera->getScreenSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_renderVisitor->visit(m_root.get());
}

void Scene::renderDepthMaps() {
    for (auto& light : m_lights) {
        m_depthVisitor->setupRenderState(light->getDepthMap().id(), light->getView(), light->getProjection());
        m_depthVisitor->visit(m_root.get());
    }
}
