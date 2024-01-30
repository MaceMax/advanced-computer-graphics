#include <vr/Scene.h>
#include <vr/glErrorUtil.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace vr;

std::shared_ptr<Scene> Scene::instance = nullptr;

Scene::Scene() : m_uniform_numberOfLights(-1) {
    m_camera = std::make_shared<Camera>();
    m_renderVisitor = std::make_shared<RenderVisitor>();
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
    m_root->setState(std::make_shared<State>(m_shader));
    m_lights.clear();

    if (!m_root->getState()->getShader()) {
        return false;
    }

    return true;
}

void Scene::add(std::shared_ptr<Light> light) {
    m_lights.push_back(light);
    m_root->getState()->addLight(light);
}

const LightVector& Scene::getLights() {
    return m_lights;
}

std::shared_ptr<Camera> Scene::getCamera() {
    return m_camera;
}

Scene::~Scene() {
}

void Scene::applyCamera() {
    m_camera->apply(m_shader);
}

void Scene::useProgram() {
    m_shader->use();
}

void Scene::resetTransform() {
    /*
    for (auto n : m_nodes)
        n->resetTransform();
    */
}

std::shared_ptr<Node> Scene::getNode(size_t i) {
    return m_nodes[i];
}

std::shared_ptr<Group>& Scene::getRoot() {
    return m_root;
}

BoundingBox Scene::calculateBoundingBox() {
    BoundingBox box;

    box.expand(m_root->calculateBoundingBox());
    return box;
}

void Scene::render() {
    CHECK_GL_ERROR_LINE_FILE();
    useProgram();
    CHECK_GL_ERROR_LINE_FILE();

    m_renderVisitor->visit(m_root.get());
}

void Scene::printSceneGraph() {
    std::cout << "Scene graph:" << std::endl;
    for (auto n : m_nodes) {
        std::cout << n->getName() << std::endl;
    }
}
