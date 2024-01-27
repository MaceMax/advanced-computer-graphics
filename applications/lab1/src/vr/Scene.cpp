#include <vr/Scene.h>
#include <vr/glErrorUtil.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace vr;

std::shared_ptr<Scene> Scene::instance = nullptr;

Scene::Scene() : m_uniform_numberOfLights(-1) {
    m_camera = std::make_shared<Camera>();
    m_renderVisitor = std::make_shared<RenderVisitor>();
    m_root = std::shared_ptr<Group>(new Group("root"));
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

    m_root->setState(std::make_shared<State>(m_shader));

    auto g = std::make_shared<Geometry>("Geometry");

    std::vector<glm::vec4> vertices = {
        glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // v0
        glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),   // v1
        glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),    // v2
        glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),   // v3
        glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),   // v4
        glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),    // v5
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),     // v6
        glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f)     // v7
    };

    std::vector<GLuint> indices = {
        0, 1, 2, 2, 3, 0,  // front face
        1, 5, 6, 6, 2, 1,  // right face
        5, 4, 7, 7, 6, 5,  // back face
        4, 0, 3, 3, 7, 4,  // left face
        3, 2, 6, 6, 7, 3,  // top face
        4, 5, 1, 1, 0, 4   // bottom face
    };

    std::vector<glm::vec3> normals;

    std::vector<glm::vec2> texCoords;

    g->buildGeometry(vertices, normals, texCoords, indices);
    if (!g->initShader(m_shader))
        return false;
    g->upload();

    m_root->addChild(g);

    return true;
}

void Scene::add(std::shared_ptr<Light>& light) {
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

std::shared_ptr<Group> Scene::getRoot() {
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
