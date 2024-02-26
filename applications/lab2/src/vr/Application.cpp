
#include <vr/Application.h>
#include <vr/FileSystem.h>
#include <vr/Nodes/Geometry.h>
#include <vr/Nodes/LightNode.h>
#include <vr/Nodes/Transform.h>
#include <vr/Scene/Loader.h>
#include <vr/Scene/Scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>

using namespace vr;

Application::Application(unsigned int width, unsigned height) : m_screenSize(width, height),
                                                                m_clearColor(0.1, 0.1, 0.1, 1) {
    m_fpsCounter = std::make_shared<FPSCounter>();
    m_fpsCounter->setFontScale(0.5f);
    m_fpsCounter->setColor(glm::vec4(0.2, 1.0, 1.0, 1.0));
}

bool Application::initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename) {
    m_loadedVShader = vshader_filename;
    m_loadedFShader = fshader_filename;
    m_loadedFilename = model_filename;

    m_scene = std::shared_ptr<Scene>(Scene::getInstance());

    if (!m_scene->initShaders(vshader_filename, fshader_filename))
        return false;
    getCamera()->setScreenSize(m_screenSize);

    std::shared_ptr<Group>& m_sceneRoot = m_scene->getRoot();
    if (model_filename.empty()) {
        std::shared_ptr<Group> geometryGroup = std::make_shared<Group>("default");
        geometryGroup->addChild(createDefaultGeometry(m_sceneRoot->getState()->getShader()));

        m_sceneRoot->addChild(geometryGroup);

        std::shared_ptr<Group> groundPlaneNode = std::make_shared<Group>("GroundPlane", true);
        // Always add a ground plane to the scene
        if (!load3DModelFile("models/GroundPlane/scene.gltf", groundPlaneNode, m_sceneRoot->getState()->getShader())) {
            return false;
        }
        m_sceneRoot->addChild(groundPlaneNode);
        m_scene->setGroundPlane(groundPlaneNode);
    } else {
        std::string ext = vr::FileSystem::getFileExtension(model_filename);

        // Ok lets load this as our own "scene file format"
        if (ext == "xml" || ext == "XML") {
            if (!loadSceneFile(model_filename, m_scene)) {
                return false;
            }

            if (m_scene->getRoot()->getChildren().empty()) {
                std::cerr << "Empty scene, something when wrong when loading files" << std::endl;
                return false;
            }

        } else {
            const std::shared_ptr<Shader>& root_shader = m_sceneRoot->getState()->getShader();

            if (!root_shader) {
                std::cerr << "No shader found" << std::endl;
                return false;
            }
            std::shared_ptr<Group> geometryGroup = std::make_shared<Group>(model_filename);
            if (!load3DModelFile(model_filename, geometryGroup, root_shader)) {
                return false;
            } else {
                m_sceneRoot->addChild(geometryGroup);
            }

            std::shared_ptr<Group> groundPlaneNode = std::make_shared<Group>("GroundPlane", true);
            // Always add a ground plane to the scene
            if (!load3DModelFile("models/GroundPlane/scene.gltf", groundPlaneNode, m_sceneRoot->getState()->getShader())) {
                return false;
            }
            m_sceneRoot->addChild(groundPlaneNode);
            m_scene->setGroundPlane(groundPlaneNode);
        }
    }

    if (m_scene->getLights().empty()) {
        std::cout << "No lights in scene, adding a default light" << std::endl;
        std::shared_ptr<Light> light = std::make_shared<Light>(glm::vec4(0.0, 1.0, 0.0, 0.0),
                                                               glm::vec4(0.1, 0.1, 0.1, 1.0),
                                                               glm::vec4(1.0, 1.0, 1.0, 1.0),
                                                               glm::vec4(0.8, 0.8, 0.8, 1.0));
        m_scene->add(light);

        BoundingBox box = m_scene->calculateSceneBoundingBox();
        float radius = box.getRadius();
        // Compute the diagonal and a suitable distance so we can see the whole thing
        float distance = radius * 1.5f;
        glm::vec3 eye = glm::vec3(0, radius, distance);

        glm::vec3 direction = glm::normalize(box.getCenter() - eye);

        glm::vec4 position;
        position = glm::vec4(eye + glm::vec3(3, 2, 0), position.w);
        light->setPosition(position);

        std::shared_ptr<LightNode> lightNode = std::make_shared<LightNode>(light);
        m_sceneRoot->addChild(lightNode);
    }

    // Calculate the view and projection matrices for every light and initialize the depth maps.
    BoundingBox sceneBox = m_scene->calculateSceneBoundingBox(true);
    BoundingBox sceneBounds = m_scene->calculateSceneBoundingBox(false);

    // Initialize the depth maps for all lights
    LightVector lights = m_scene->getLights();
    for (int i = 0; i < lights.size(); i++)
        lights[i]->init(i, sceneBox, sceneBounds.getRadius());

#if 0
  std::shared_ptr<Mesh> ground(new Mesh);

  BoundingBox box = m_sceneRoot->calculateBoundingBox();
  glm::vec3 size = box.max() - box.min();
  // mesh position initialized in init_view()

  ground->vertices.push_back(glm::vec4(size.x, 0, size.y, 1.0));
  ground->vertices.push_back(glm::vec4(size.x, 0, -size.y, 1.0));
  ground->vertices.push_back(glm::vec4(-size.x, 0, size.y, 1.0));
  ground->vertices.push_back(glm::vec4(-size.x, 0, size.y, 1.0));
  ground->vertices.push_back(glm::vec4(size.x, 0, -size.y, 1.0));
  ground->vertices.push_back(glm::vec4(-size.x, 0, -size.y, 1.0));
  
  for (unsigned int k = 0; k < 6; k++)
    ground->normals.push_back(glm::vec3(0.0, 1.0, 0.0));

  std::shared_ptr<Node> groundNode = std::shared_ptr<Node>(new Node);
  groundNode->add(ground);

  groundNode->setInitialTransform(glm::translate(glm::mat4(), glm::vec3(box.getCenter().x, box.min().y, box.getCenter().z)));
  ground->name = "ground";

  m_sceneRoot->add(groundNode);
#endif

    return 1;
}

void Application::reloadScene() {
    initResources(m_loadedFilename, m_loadedVShader, m_loadedFShader);
    initView();
}

void Application::setClearColor(const glm::f32vec4& clearColor) {
    m_clearColor = clearColor;
}

void Application::initView() {
    // Compute a bounding box around the whole scene
    BoundingBox box = m_scene->calculateSceneBoundingBox();
    float radius = box.getRadius();

    // Compute the diagonal and a suitable distance so we can see the whole thing
    float distance = radius * 1.5f;
    glm::vec3 eye = glm::vec3(0, radius, distance);

    glm::vec3 direction = glm::normalize(box.getCenter() - eye);

    // Set the position/direction of the camera
    getCamera()->set(eye, direction, glm::vec3(0.0, 1.0, 0.0));
    getCamera()->setNearFar(glm::vec2(0.1, distance * 100));

    // Compute the default movement speed based on the radius of the scene
    getCamera()->setSpeed(0.7f * radius);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void Application::render(GLFWwindow* window) {
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);

    m_scene->render();
    m_fpsCounter->render(window);
}

void Application::update(GLFWwindow* window) {
    m_scene->useProgram();

    render(window);
}

void Application::selectLight(int idx) {
    m_scene->setSelectedLight(idx);
}

void Application::processInput(GLFWwindow* window) {
    getCamera()->processInput(window);

    float currentTime = glfwGetTime();
    float deltaTime = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;

    glm::vec4 deltaPosition = glm::vec4(0.0f);

    std::shared_ptr<Light> light = m_scene->getSelectedLight();
    glm::vec4 position = light->getPosition();

    float translationSpeed = position.w == 0 ? TRANSLATION_SPEED : TRANSLATION_SPEED * 10.0f;

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        deltaPosition.x -= translationSpeed * deltaTime;  // Move light source along the negative x-axis

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        deltaPosition.x += translationSpeed * deltaTime;  // Move light source along the positive x-axis

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        deltaPosition.y += translationSpeed * deltaTime;  // Move light source along the positive y-axis

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        deltaPosition.y -= translationSpeed * deltaTime;  // Move light source along the negative y-axis

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        deltaPosition.z -= translationSpeed * deltaTime;  // Move light source along the negative z-axis

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        deltaPosition.z += translationSpeed * deltaTime;  // Move light source along the positive z-axis

    if (glm::length(deltaPosition) > 0.0f) {
        position += deltaPosition;
        light->setPosition(position);
    }
}

void Application::toggleShadows() {
    m_scene->toggleShadows();
}

void Application::setScreenSize(unsigned int width, unsigned int height) {
    getCamera()->setScreenSize(glm::uvec2(width, height));
    glViewport(0, 0, width, height);
}

std::shared_ptr<Camera> Application::getCamera() {
    return m_scene->getCamera();
}

std::shared_ptr<Geometry> Application::createDefaultGeometry(const std::shared_ptr<Shader>& shader) {
    std::shared_ptr<Geometry> geometry = std::make_shared<Geometry>();
    geometry->setState(std::make_shared<State>(shader));

    // Should implement so that geometry can be created manually in the xml scene file
    std::vector<glm::vec4> vertices = {
        // Front face
        glm::vec4(-1.0, -1.0, 1.0, 1.0), glm::vec4(1.0, -1.0, 1.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4(-1.0, 1.0, 1.0, 1.0),
        // Back face
        glm::vec4(-1.0, -1.0, -1.0, 1.0), glm::vec4(-1.0, 1.0, -1.0, 1.0), glm::vec4(1.0, 1.0, -1.0, 1.0), glm::vec4(1.0, -1.0, -1.0, 1.0),
        // Left face
        glm::vec4(-1.0, -1.0, -1.0, 1.0), glm::vec4(-1.0, -1.0, 1.0, 1.0), glm::vec4(-1.0, 1.0, 1.0, 1.0), glm::vec4(-1.0, 1.0, -1.0, 1.0),
        // Right face
        glm::vec4(1.0, -1.0, -1.0, 1.0), glm::vec4(1.0, 1.0, -1.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4(1.0, -1.0, 1.0, 1.0),
        // Bottom face
        glm::vec4(-1.0, -1.0, -1.0, 1.0), glm::vec4(1.0, -1.0, -1.0, 1.0), glm::vec4(1.0, -1.0, 1.0, 1.0), glm::vec4(-1.0, -1.0, 1.0, 1.0),
        // Top face
        glm::vec4(-1.0, 1.0, -1.0, 1.0), glm::vec4(-1.0, 1.0, 1.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4(1.0, 1.0, -1.0, 1.0)};

    std::vector<glm::vec3> normals = {
        // Front face
        glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0),
        // Back face
        glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0),
        // Left face
        glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0),
        // Right face
        glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
        // Bottom face
        glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0),
        // Top face
        glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)};

    std::vector<GLuint> indices = {
        // Front face
        0, 1, 2, 0, 2, 3,
        // Back face
        4, 5, 6, 4, 6, 7,
        // Left face
        8, 9, 10, 8, 10, 11,
        // Right face
        12, 13, 14, 12, 14, 15,
        // Bottom face
        16, 17, 18, 16, 18, 19,
        // Top face
        20, 21, 22, 20, 22, 23};

    std::vector<glm::vec2> texCoords;
    geometry->buildGeometry(vertices, normals, texCoords, indices);
    geometry->initShader(shader);
    geometry->upload();

    return geometry;
}

Application::~Application() {
    m_scene->cleanup();
}