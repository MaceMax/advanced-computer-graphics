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

    void renderToQuad(std::shared_ptr<Texture> texture, int x, int y, int width, int height, bool debug);

    void renderDebug();

    void toogleDebug();

    void render(GLFWwindow* window);

    void update(GLFWwindow* window);

    void processInput(GLFWwindow* window);

    void setClearColor(const glm::f32vec4& clearColor);

    std::shared_ptr<Geometry> createDefaultGeometry(const std::shared_ptr<Shader>& shader);

    void toggleShadows();

    void selectLight(int idx);

    void changeCamera(int next);

    void changeDebugTexture(int next);

   private:
    // Parent of all to be rendered
    std::shared_ptr<Scene> m_scene;

    std::shared_ptr<FPSCounter> m_fpsCounter;
    std::string m_loadedFilename, m_loadedVShader, m_loadedFShader;
    glm::uvec2 m_screenSize;
    glm::f32vec4 m_clearColor;
    float m_lastFrameTime = 0.0f;

    bool m_debug = false;
    int m_debugTexture = 0;

    std::shared_ptr<Shader> m_quad_shader;
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
