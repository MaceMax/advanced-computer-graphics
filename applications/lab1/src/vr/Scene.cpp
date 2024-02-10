#include <vr/Animation/CircleAnimation.h>
#include <vr/Group.h>
#include <vr/Scene.h>
#include <vr/glErrorUtil.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace vr;

std::shared_ptr<Scene> Scene::instance = nullptr;

Scene::Scene() : m_uniform_numberOfLights(-1) {
    m_camera = std::make_shared<Camera>();
    m_renderVisitor = std::make_shared<RenderVisitor>();
    m_renderVisitor->setActiveCamera(m_camera);
    m_updateVisitor = std::make_shared<UpdateVisitor>();
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
    Light light = Light(glm::vec4(0.0, -2.0, 2.0, 0.0),
                        glm::vec4(1, 1, 1, 1),
                        glm::vec4(1, 1, 1, 1));
    std::shared_ptr<Light> light1 = std::make_shared<Light>(light);
    state->addLight(light1);
    state->setMaterial(std::make_shared<Material>());
    state->setLightingEnabled(true);
    m_root->setState(state);

    return true;
}

void Scene::add(std::shared_ptr<Light> light) {
    m_root->getState()->addLight(light);
}

const LightVector Scene::getLights() {
    return m_root->getState()->getLights();
}

std::shared_ptr<Camera> Scene::getCamera() {
    return m_camera;
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

    box.expand(m_root->calculateBoundingBox(glm::mat4(1.0f)));
    return box;
}

void Scene::render() {
    m_updateVisitor->visit(m_root.get());
    m_renderVisitor->visit(m_root.get());
}

void Scene::printSceneGraph() {
    std::cout << "Scene graph:" << std::endl;
    for (auto n : m_nodes) {
        std::cout << n->getName() << std::endl;
    }
}
