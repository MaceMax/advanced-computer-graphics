#pragma once

#include <memory>

// clang-format off
#include "vr/Scene/Camera.h"
#include "vr/FPSCounter.h"
#include "vr/Scene/Scene.h"
#define TRANSLATION_SPEED 8.5f

namespace vr {

/**
Class that holds a scene and a camera.
*/
class Application {
   public:
    /**
    Constructor
    */
    Application(unsigned int width, unsigned int height);

    /**
    Destructor
    */
    ~Application();

    bool initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename);
    void initView();

    void setScreenSize(unsigned int width, unsigned int height);

    std::shared_ptr<Camera> getCamera();

    void reloadScene();

    void renderToQuad(std::shared_ptr<Texture> texture, int x, int y, int width, int height);

    void drawQuad();

    void renderDebug();

    void toogleDebug();

    void render(GLFWwindow* window);

    void update(GLFWwindow* window);

    void processInput(GLFWwindow* window);

    void setClearColor(const glm::f32vec4& clearColor);

    std::shared_ptr<Geometry> createDefaultGeometry(const std::shared_ptr<Shader>& shader);

    void toggleShadows();

    void toggleBloom();

    void toggleDOF();

    void changeFocus(float delta);

    void selectLight(int idx);

    void changeCamera(int next);

    void changeDebugTexture(int next);

    void renderToTexture();

    void blurTexture(const std::shared_ptr<Texture> input, std::shared_ptr<Texture> output, float blurAmount);

   private:
    // Parent of all to be rendered
    std::shared_ptr<Scene> m_scene;

    std::shared_ptr<FPSCounter> m_fpsCounter;
    std::string m_loadedFilename, m_loadedVShader, m_loadedFShader;
    glm::uvec2 m_screenSize;
    glm::f32vec4 m_clearColor;
    float m_lastFrameTime = 0.0f;

    bool m_debug = false;
    bool m_bloom = false;
    bool m_dof = false;
    int m_debugTexture = 0;

    float m_focus = 0.01f;
    float m_aperture = 0.5f;

    std::shared_ptr<Shader> m_quad_shader, m_scene_shader, m_gaussian_shader;

    GLuint m_fbo, m_pingpongFBO[2];
    std::shared_ptr<Texture> m_pingpongTextures[2];
    std::shared_ptr<Texture> m_sceneTexture, m_bloomTexture, m_brightTexture, m_blurTexture;

    GLuint m_attribute_vertex, m_attribute_texcoord;
    GLuint m_quad_vao, m_quad_vbo_vertices, m_quad_vbo_texcoords;

    float m_quad_vertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
};
}  // namespace vr
