#include <vr/Nodes/Group.h>
#include <vr/Scene/Scene.h>
#include <vr/glErrorUtil.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace vr;

std::shared_ptr<Scene> Scene::instance = nullptr;

Scene::Scene() {
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

    return true;
}

void Scene::initScene(unsigned int width, unsigned int height) {
    m_lights.clear();
    m_cameras.clear();
    m_camera = std::make_shared<Camera>();
    m_camera->setScreenSize(glm::uvec2(width, height));
    m_cameras.push_back(m_camera);

    m_gbuffer = std::make_shared<Gbuffer>(width, height);

    m_renderVisitor = std::make_shared<RenderVisitor>();
    m_updateVisitor = std::make_shared<UpdateVisitor>();
    m_depthVisitor = std::make_shared<DepthVisitor>();

    m_renderVisitor->setActiveCamera(m_camera);
    m_updateVisitor->setActiveCamera(m_camera);
    m_depthVisitor->setActiveCamera(m_camera);

    m_root = std::shared_ptr<Group>(new Group("root"));

    // Create a default state. Can be overridden by the user in the scene file.
    std::shared_ptr<State> state = std::make_shared<State>(m_shader);
    state->setMaterial(std::make_shared<Material>());
    state->setLightingEnabled(true);
    state->setShadowEnabled(true);
    m_root->setState(state);
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
    if (m_shader)
        m_shader = nullptr;

    if (m_camera)
        m_camera = nullptr;

    if (m_root)
        m_root = nullptr;

    if (m_depthVisitor)
        m_depthVisitor = nullptr;

    if (m_renderVisitor)
        m_renderVisitor = nullptr;

    if (m_lights.size() > 0)
        m_lights.clear();

    if (m_groundPlane)
        m_groundPlane = nullptr;

    if (m_gbuffer)
        m_gbuffer = nullptr;
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

void Scene::setSelectedLight(int index) {
    selectedLight = index;
    if (index < 0 || index >= m_lights.size()) {
        selectedLight = m_lights.size() - 1;
    }
}

std::shared_ptr<Light> Scene::getSelectedLight() {
    return m_lights[selectedLight];
}

void Scene::addCamera(std::shared_ptr<Camera> camera) {
    m_cameras.push_back(camera);
}

std::shared_ptr<Camera> Scene::getActiveCamera() {
    return m_camera;
}

void Scene::setActiveCamera(int next) {
    int next_index = selectedCamera + next;

    if (next_index < 0) {
        next_index = m_cameras.size() - 1;
    } else if (next_index >= m_cameras.size()) {
        next_index = 0;
    }

    selectedCamera = next_index;
    m_camera = m_cameras[selectedCamera];
    m_renderVisitor->setActiveCamera(m_camera);
    m_updateVisitor->setActiveCamera(m_camera);
    m_depthVisitor->setActiveCamera(m_camera);
}

CameraVector Scene::getCameras() {
    return m_cameras;
}

std::shared_ptr<Texture> Scene::getGbufferTexture(unsigned int textureType) {
    switch (textureType) {
        case GBUFFER_POSITION:
            return m_gbuffer->getPosition();
        case GBUFFER_NORMAL:
            return m_gbuffer->getNormal();
        case GBUFFER_ALBEDO:
            return m_gbuffer->getAlbedo();
        case GBUFFER_SPECULAR:
            return m_gbuffer->getAlbedo();
        case GBUFFER_DEPTH:
            return m_gbuffer->getDepth();
        case GBUFFER_METALLIC_ROUGHNESS:
            return m_gbuffer->getMetallicRoughness();
        default:
            std::cerr << "Invalid texture type" << std::endl;
            return nullptr;
    }
}

void Scene::rescaleGbuffer(int width, int height) {
    m_gbuffer->rescaleFramebuffer(width, height);
}

std::shared_ptr<Gbuffer> Scene::getGbuffer() {
    return m_gbuffer;
}

BoundingBox
Scene::calculateSceneBoundingBox(bool excludeGround) {
    BoundingBox box;

    if (m_groundPlane != nullptr)
        m_groundPlane->setExclude(excludeGround);

    box.expand(m_root->calculateBoundingBox(glm::mat4(1.0f)));

    if (!excludeGround && m_groundPlane != nullptr)
        m_groundRadius = box.getRadius();

    // Should be excluded by default
    if (m_groundPlane != nullptr)
        m_groundPlane->setExclude(true);

    return box;
}

void Scene::render() {
    m_updateVisitor->visit(m_root.get());

    // if (m_shadowsEnabled)
    //     renderDepthMaps(m_updateVisitor->sceneChanged());

    m_updateVisitor->setSceneChanged(false);

    m_gbuffer->bindFBO();
    glViewport(0, 0, m_camera->getScreenSize().x, m_camera->getScreenSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_renderVisitor->visit(m_root.get());
    m_gbuffer->unbindFBO();
}

void Scene::renderDepthMaps(bool sceneChanged) {
    BoundingBox sbox;
    if (sceneChanged)
        sbox = calculateSceneBoundingBox(true);

    for (auto& light : m_lights) {
        if (sceneChanged)
            light->setShadowParams(sbox.getRadius(), sbox.getCenter(), m_groundRadius);

        m_depthVisitor->setupRenderState(light);
        m_depthVisitor->visit(m_root.get());
    }
}
