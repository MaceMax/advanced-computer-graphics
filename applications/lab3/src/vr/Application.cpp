
#include <vr/Application.h>
#include <vr/FileSystem.h>
#include <vr/Nodes/Geometry.h>
#include <vr/Nodes/LightNode.h>
#include <vr/Nodes/Transform.h>
#include <vr/Scene/Loader.h>
#include <vr/Scene/Scene.h>
#include <vr/glErrorUtil.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <random>

using namespace vr;

Application::Application(unsigned int width, unsigned height) : m_screenSize(width, height),
                                                                m_clearColor(0.0, 0.0, 0.0, 1) {
    m_fpsCounter = std::make_shared<FPSCounter>();
    m_fpsCounter->setFontScale(0.5f);
    m_fpsCounter->setColor(glm::vec4(0.2, 1.0, 1.0, 1.0));

    m_quad_shader = std::make_shared<Shader>("shaders/quad-shader.vs", "shaders/quad-shader.fs");

    initTextureBuffers();

    // Create framebuffer for scene and bright textures
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_sceneTexture->id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_brightTexture->id(), 0);

    GLenum attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete" << std::endl;
        exit(1);
    }

    // Create framebuffers for gaussian blur
    glGenFramebuffers(2, m_pingpongFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTextures[0]->id(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[1]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTextures[1]->id(), 0);

    // Create framebuffers for SSAO
    glGenFramebuffers(2, m_ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO[0]);  // Color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoBuffer->id(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO[1]);  // Blur Color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoBlurBuffer->id(), 0);

    const char* attribs[] = {"vertex_position", "vertex_texCoord"};
    m_attribute_vertex = m_quad_shader->getAttribute(attribs[0]);
    m_attribute_texcoord = m_quad_shader->getAttribute(attribs[1]);

    if (m_attribute_vertex == -1 || m_attribute_texcoord == -1) {
        std::cerr << "Error: Could not get attribute" << std::endl;
        exit(1);
    }

    glGenVertexArrays(1, &m_quad_vao);
    glBindVertexArray(m_quad_vao);

    glGenBuffers(1, &m_quad_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo_vertices);
    GLfloat quad_vertices[] = {-1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_quad_vbo_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo_texcoords);
    GLfloat quad_texcoords[] = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_texcoords), quad_texcoords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(m_attribute_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo_vertices);
    glVertexAttribPointer(m_attribute_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(m_attribute_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo_texcoords);
    glVertexAttribPointer(m_attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

void Application::initTextureBuffers() {
    m_sceneTexture = std::make_shared<Texture>();
    m_brightTexture = std::make_shared<Texture>();
    m_bloomTexture = std::make_shared<Texture>();
    m_blurTexture = std::make_shared<Texture>();
    m_pingpongTextures[0] = std::make_shared<Texture>();
    m_pingpongTextures[1] = std::make_shared<Texture>();
    m_ssaoBuffer = std::make_shared<Texture>();
    m_ssaoBlurBuffer = std::make_shared<Texture>();
    m_ssaoNoiseTexture = std::make_shared<Texture>();

    m_sceneTexture->createFramebufferTexture(SCREEN_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);
    m_brightTexture->createFramebufferTexture(BLOOM_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);
    m_bloomTexture->createFramebufferTexture(BLOOM_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);
    m_blurTexture->createFramebufferTexture(BLUR_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);
    m_ssaoBuffer->createFramebufferTexture(SSAO_COLOR_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);
    m_ssaoBlurBuffer->createFramebufferTexture(SSAO_BLUR_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);

    m_ssaoNoiseTexture->createNoiseTexture(4, 4, generateSSAONoise(16));

    m_pingpongTextures[0]->createFramebufferTexture(PING_PONG_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);
    m_pingpongTextures[1]->createFramebufferTexture(PING_PONG_TEXTURE_SLOT, m_screenSize.x, m_screenSize.y);
}

bool Application::initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename) {
    m_loadedVShader = vshader_filename;
    m_loadedFShader = fshader_filename;
    m_loadedFilename = model_filename;

    m_scene_shader = std::make_shared<Shader>("shaders/scene-shader.vs", "shaders/scene-shader.fs");
    m_quad_shader = std::make_shared<Shader>("shaders/quad-shader.vs", "shaders/quad-shader.fs");
    m_gaussian_shader = std::make_shared<Shader>("shaders/gaussian-shader.vs", "shaders/gaussian-shader.fs");
    m_ssao_shader = std::make_shared<Shader>("shaders/ssao-shader.vs", "shaders/ssao-shader.fs");
    m_ssao_blur_shader = std::make_shared<Shader>("shaders/ssao-shader.vs", "shaders/ssao-blur-shader.fs");

    m_ssaoKernel = generateSSAOKernel(64);

    if (!m_quad_shader->valid() || !m_scene_shader->valid() || !m_gaussian_shader->valid() || !m_ssao_shader->valid() || !m_ssao_blur_shader->valid()) {
        std::cerr << "Error: Could not load shaders" << std::endl;
        return false;
    }

    m_scene = std::shared_ptr<Scene>(Scene::getInstance());
    m_scene->cleanup();
    if (!m_scene->initShaders(vshader_filename, fshader_filename))
        return false;

    m_scene->initScene(m_screenSize.x, m_screenSize.y);

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

    // Initialize the view and projection matrices for the lights
    LightVector lights = m_scene->getLights();
    for (int i = 0; i < lights.size(); i++)
        lights[i]->init(sceneBox, sceneBounds.getRadius());

    // Initialize the depth map arrays
    m_scene->initDepthMaps();

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
    for (auto& camera : m_scene->getCameras()) {
        camera->setSpeed(0.7f * radius);
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void Application::drawQuad() {
    glBindVertexArray(m_quad_vao);
    glEnableVertexAttribArray(m_attribute_vertex);
    glEnableVertexAttribArray(m_attribute_texcoord);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(m_attribute_vertex);
    glDisableVertexAttribArray(m_attribute_texcoord);
    glBindVertexArray(0);
}

void Application::blurTexture(const std::shared_ptr<Texture> input, std::shared_ptr<Texture> output, float blurAmount) {
    bool horizontal = true, first_iteration = true;
    int iter = 10;  // Amount of gaussian blur iterations

    m_pingpongTextures[!horizontal] = output;
    glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[!horizontal]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTextures[!horizontal]->id(), 0);

    m_gaussian_shader->use();
    for (int i = 0; i < iter; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[horizontal]);
        if (first_iteration)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_gaussian_shader->setBool("horizontal", horizontal);

        first_iteration ? input->bind() : m_pingpongTextures[!horizontal]->bind();
        first_iteration ? m_gaussian_shader->setInt("image", input->slot()) : m_gaussian_shader->setInt("image", m_pingpongTextures[!horizontal]->slot());

        drawQuad();

        m_pingpongTextures[!horizontal]->unbind();

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::renderToQuad(std::shared_ptr<Texture> texture, int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    m_quad_shader->use();

    m_quad_shader->setBool("bloomEnabled", m_bloom && texture->slot() == SCREEN_TEXTURE_SLOT);
    if (m_bloom && texture->slot() == SCREEN_TEXTURE_SLOT) {
        blurTexture(m_brightTexture, m_bloomTexture, 10.0f);
        m_quad_shader->use();
        m_quad_shader->setInt("bloomTexture", m_bloomTexture->slot());
        m_bloomTexture->bind();
    }

    m_quad_shader->setBool("dofEnabled", m_dof && texture->slot() == SCREEN_TEXTURE_SLOT);
    if (m_dof && texture->slot() == SCREEN_TEXTURE_SLOT) {
        blurTexture(m_sceneTexture, m_blurTexture, 10.0f);
        m_quad_shader->use();
        m_quad_shader->setFloat("focusDistance", m_focus);
        m_quad_shader->setFloat("aperture", m_aperture);
        m_quad_shader->setInt("blurTexture", m_blurTexture->slot());
        m_quad_shader->setInt("depthTexture", m_scene->getGbufferTexture(GBUFFER_DEPTH)->slot());
        m_blurTexture->bind();
        m_scene->getGbufferTexture(GBUFFER_DEPTH)->bind();
    }

    m_quad_shader->setFloat("near", getCamera()->getNear());
    m_quad_shader->setFloat("far", getCamera()->getFar());
    m_quad_shader->setInt("screenTexture", texture->slot());
    m_quad_shader->setBool("isDepth", texture->slot() == G_BUFFER_DEPTH_SLOT);
    texture->bind();

    drawQuad();

    if (m_bloom && texture->slot() == SCREEN_TEXTURE_SLOT)
        m_bloomTexture->unbind();

    if (m_dof && texture->slot() == SCREEN_TEXTURE_SLOT) {
        m_blurTexture->unbind();
        m_scene->getGbufferTexture(GBUFFER_DEPTH)->unbind();
    }

    texture->unbind();
}

void Application::renderDebug() {
    std::shared_ptr<Texture> texture = m_scene->getGbufferTexture(m_debugTexture);
    // Render debug window at the top right corner
    renderToQuad(texture, m_screenSize.x - m_screenSize.x / 3, m_screenSize.y - m_screenSize.y / 3, m_screenSize.x / 3, m_screenSize.y / 3);
    glViewport(0, 0, m_screenSize.x, m_screenSize.y);
}

void Application::renderSSAO() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO[0]);
    glClear(GL_COLOR_BUFFER_BIT);
    m_ssao_shader->use();
    for (int i = 0; i < m_ssaoKernel.size(); i++) {
        m_ssao_shader->setVec3("samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);
    }
    m_ssao_shader->setMat4("p", getCamera()->getProjection());
    m_ssao_shader->setMat4("v", getCamera()->getView());
    m_ssao_shader->setInt("kernelSize", m_ssaoKernel.size());
    m_ssao_shader->setFloat("screenX", m_screenSize.x);
    m_ssao_shader->setFloat("screenY", m_screenSize.y);

    m_scene->getGbufferTexture(GBUFFER_POSITION)->bind();
    m_scene->getGbufferTexture(GBUFFER_NORMAL)->bind();
    m_ssaoNoiseTexture->bind();
    m_ssao_shader->setInt("gPosition", G_BUFFER_POSITION_SLOT);
    m_ssao_shader->setInt("gNormal", G_BUFFER_NORMAL_SLOT);
    m_ssao_shader->setInt("noiseTexture", m_ssaoNoiseTexture->slot());

    drawQuad();

    m_scene->getGbufferTexture(GBUFFER_POSITION)->unbind();
    m_scene->getGbufferTexture(GBUFFER_NORMAL)->unbind();
    m_ssaoNoiseTexture->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::renderToTexture() {
    m_scene_shader->use();
    glViewport(0, 0, m_screenSize.x, m_screenSize.y);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    m_scene->getGbuffer()->bindTextures();
    m_scene_shader->setInt("gPositionAmbient", G_BUFFER_POSITION_SLOT);
    m_scene_shader->setInt("gNormalAmbient", G_BUFFER_NORMAL_SLOT);
    m_scene_shader->setInt("gAlbedoAmbient", G_BUFFER_ALBEDO_SLOT);
    m_scene_shader->setInt("gAoMetallicRoughness", G_BUFFER_METALLIC_ROUGHNESS);
    m_scene_shader->setVec3("viewPos", getCamera()->getPosition());
    m_scene_shader->setBool("shadowsEnabled", m_scene->shadowsEnabled());

    LightVector lights = m_scene->getLights();

    int pointLightIndex = 0;
    int directionalLightIndex = 0;

    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]->getPosition().w == 0.0) {
            lights[i]->apply(m_scene_shader, i, m_scene->shadowsEnabled());
            m_scene->getDirectionalShadowMap()->bind();
            m_scene_shader->setInt("directionalShadowMaps", m_scene->getDirectionalShadowMap()->slot());
            m_scene_shader->setInt("lights[" + std::to_string(i) + "].shadowMapIndex", directionalLightIndex);
            directionalLightIndex++;
        } else {
            lights[i]->apply(m_scene_shader, i, m_scene->shadowsEnabled());
            m_scene->getPointShadowMap()->bind();
            m_scene_shader->setInt("pointShadowMaps", m_scene->getPointShadowMap()->slot());
            m_scene_shader->setInt("lights[" + std::to_string(i) + "].shadowMapIndex", pointLightIndex);
            pointLightIndex++;
        }
    }

    drawQuad();

    m_scene->getGbuffer()->unbindTextures();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // renderSSAO();
}

void Application::render(GLFWwindow* window) {
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    m_scene->render();

    renderToTexture();

    renderToQuad(m_sceneTexture, 0, 0, m_screenSize.x, m_screenSize.y);

    if (m_debug)
        renderDebug();

    m_fpsCounter->render(window);
}

void Application::update(GLFWwindow* window) {
    m_scene->useProgram();
    render(window);
}

void Application::processInput(GLFWwindow* window) {
    getCamera()->processInput(window);

    float currentTime = glfwGetTime();
    float deltaTime = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;

    glm::vec4 deltaPosition = glm::vec4(0.0f);

    std::shared_ptr<Light> light = m_scene->getSelectedLight();
    glm::vec4 position = light->getPosition();

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        deltaPosition.x -= TRANSLATION_SPEED * deltaTime;  // Move light source along the negative x-axis

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        deltaPosition.x += TRANSLATION_SPEED * deltaTime;  // Move light source along the positive x-axis

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        deltaPosition.y += TRANSLATION_SPEED * deltaTime;  // Move light source along the positive y-axis

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        deltaPosition.y -= TRANSLATION_SPEED * deltaTime;  // Move light source along the negative y-axis

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        deltaPosition.z -= TRANSLATION_SPEED * deltaTime;  // Move light source along the negative z-axis

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        deltaPosition.z += TRANSLATION_SPEED * deltaTime;  // Move light source along the positive z-axis

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        if (m_focus + deltaTime * 0.1f < 1.0f) m_focus += deltaTime * 0.1f;

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        if (m_focus - deltaTime * 0.1f > 0.0f) m_focus -= deltaTime * 0.1f;

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        if (m_aperture + deltaTime * 0.1f < 1.0f) m_aperture += deltaTime * 0.1f;  // Higher aperture value means more blur

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        if (m_aperture - deltaTime * 0.1f > 0.01f) m_aperture -= deltaTime * 0.1f;  // Lower aperture value means less blur

    if (glm::length(deltaPosition) > 0.0f) {
        position += deltaPosition;
        light->setPosition(position);
    }
}

void Application::selectLight(int idx) {
    m_scene->setSelectedLight(idx - 1);
}

void Application::changeCamera(int next) {
    m_scene->setActiveCamera(next);
    getCamera()->setScreenSize(m_screenSize);
}

void Application::changeDebugTexture(int next) {
    m_debugTexture += next;

    if (m_debugTexture < 0) {
        m_debugTexture = GBUFFER_METALLIC_ROUGHNESS;
    }

    if (m_debugTexture > GBUFFER_METALLIC_ROUGHNESS) {
        m_debugTexture = GBUFFER_POSITION;
    }
}

void Application::toggleShadows() {
    m_scene->toggleShadows();
}

void Application::toggleBloom() {
    m_bloom = !m_bloom;
}

void Application::toggleDOF() {
    m_dof = !m_dof;
}

void Application::toogleDebug() {
    m_debug = !m_debug;
}

void Application::toggleLight() {
    m_scene->getSelectedLight()->toggleEnabled();
}

void Application::setScreenSize(unsigned int width, unsigned int height) {
    m_screenSize = glm::uvec2(width, height);
    getCamera()->setScreenSize(glm::uvec2(width, height));
    m_scene->rescaleGbuffer(width, height);
    m_sceneTexture->rescale(width, height);
    m_brightTexture->rescale(width, height);
    m_bloomTexture->rescale(width, height);
    m_blurTexture->rescale(width, height);
    m_pingpongTextures[0]->rescale(width, height);
    m_pingpongTextures[1]->rescale(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_sceneTexture->id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_brightTexture->id(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTextures[0]->id(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[1]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTextures[1]->id(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<Camera> Application::getCamera() {
    return m_scene->getActiveCamera();
}

std::shared_ptr<Geometry> Application::createDefaultGeometry(const std::shared_ptr<Shader>& shader) {
    std::shared_ptr<Geometry> geometry = std::make_shared<Geometry>();
    std::shared_ptr<Material> material = std::make_shared<Material>();
    std::shared_ptr<State> state = std::make_shared<State>(shader);
    state->setMaterial(material);

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

std::vector<glm::vec3> Application::generateSSAOKernel(int kernelSize) {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (int i = 0; i < kernelSize; i++) {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = (float)i / kernelSize;

        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    return ssaoKernel;
}

std::vector<glm::vec3> Application::generateSSAONoise(int noiseSize) {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < noiseSize; i++) {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }

    return ssaoNoise;
}

float Application::lerp(float a, float b, float f) {
    return a + f * (b - a);
}

Application::~Application() {
    m_scene->cleanup();
}