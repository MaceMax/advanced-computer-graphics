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

    /**
     * @brief Initialize the resources for the application
     * 
     * @param model_filename  The filename of the model to load
     * @param vshader_filename  The filename of the vertex shader to load
     * @param fshader_filename The filename of the fragment shader to load
     * @return true if the resources were loaded successfully, false otherwise
     */
    bool initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename);

    /**
     * @brief Initializes the view to appropriate values based on the scene.
     */
    void initView();

    /**
     * @brief Initializes the texture used for framebuffers buffers
     */
    void initTextureBuffers();

    /**
     * @brief Updates the values of the screen size
     * 
     * @param width  The new width
     * @param height  The new height    
     */
    void setScreenSize(unsigned int width, unsigned int height);

    /**
     * @brief Get the selected camera
     * 
     * @return std::shared_ptr<Camera>  The selected camera
     */
    std::shared_ptr<Camera> getCamera();

    /**
     * @brief Reloads the scene, meaning all shaders and models are reloaded
     */
    void reloadScene();

    /**
     * @brief Renders a texture to a quad
     * 
     * @param texture  The texture to render
     * @param x  The x position of the quad
     * @param y  The y position of the quad
     * @param width  The width of the quad
     * @param height  The height of the quad
     */
    void renderToQuad(std::shared_ptr<Texture> texture, int x, int y, int width, int height);

    /**
     * @brief Renders a quad
     */
    void drawQuad();

    /**
     * @brief Renders a debug quad
     */
    void renderDebug();

    /**
     * @brief Toggles the debug mode
     */
    void toogleDebug();

    /**
     * @brief Renders the scene
     * 
     * @param window  The window to render to
     */
    void render(GLFWwindow* window);

    /**
     * @brief Updates the application
     * 
     * @param window  The window to update
     */
    void update(GLFWwindow* window);

    /**
     * @brief Processes input
     * 
     * @param window  The window to process input for
     */
    void processInput(GLFWwindow* window);

    /**
     * @brief Sets the clear color
     * 
     * @param clearColor  The new clear color
     */
    void setClearColor(const glm::f32vec4& clearColor);

    /**
     * @brief Creates a default geometry
     * 
     * @param shader  The shader to use
     * @return std::shared_ptr<Geometry>  The created geometry
     */
    std::shared_ptr<Geometry> createDefaultGeometry(const std::shared_ptr<Shader>& shader);

    /**
     * @brief Toggle shadows
     */
    void toggleShadows();

    /**
     * @brief Toggle bloom
     */
    void toggleBloom();

    /**
     * @brief Toggle depth of field
     */
    void toggleDOF();

    /**
     * @brief Toggle the selected light
     */
    void toggleLight();

    /**
     * @brief Selects a light
     * 
     * @param idx  The index of the light to select
     */    
    void selectLight(int idx);

    /**
     * @brief Changes the camera
     * 
     * @param next  The index of the next camera
     */
    void changeCamera(int next);

    /**
     * @brief Changes the debug texture that will be rendered to the debug quad
     * 
     * @param next  The index of the next debug texture
     */
    void changeDebugTexture(int next);

    /**
     * @brief Combines the gbuffer textures and does lighting calculations and renders to a texture 
     */
    void renderToTexture();

    /**
     * @brief Blurs a texture
     * 
     * @param input The texture to blur
     * @param output The texture to output to
     * @param blurAmount The amount to blur
     */
    void blurTexture(const std::shared_ptr<Texture> input, std::shared_ptr<Texture> output, float blurAmount);

    /**
     * @brief Adds SSAO to the scene. Could unfortunately not get this to work properly.
     */
    void renderSSAO();

    /**
     * @brief Generates a kernel for SSAO
     * 
     * @param kernelSize  The size of the kernel
     * @return std::vector<glm::vec3> The generated kernel
     */
    std::vector<glm::vec3> generateSSAOKernel(int kernelSize);

    /**
     * @brief Generates noise for SSAO
     * 
     * @param noiseSize  The size of the noise
     * @return std::vector<glm::vec3> The generated noise
     */
    std::vector<glm::vec3> generateSSAONoise(int noiseSize);

    /**
     * @brief Linearly interpolates between two values
     * 
     * @param a the first value
     * @param b the second value
     * @param f the interpolation factor
     * @return float the interpolated value
     */
    float lerp(float a, float b, float f);

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

    std::shared_ptr<Shader> m_quad_shader, m_scene_shader, m_gaussian_shader, m_ssao_shader, m_ssao_blur_shader;

    GLuint m_fbo, m_pingpongFBO[2], m_ssaoFBO[2];
    std::vector<glm::vec3> m_ssaoKernel;
    std::shared_ptr<Texture> m_ssaoNoiseTexture, m_ssaoBuffer, m_ssaoBlurBuffer;
    std::shared_ptr<Texture> m_pingpongTextures[2];
    std::shared_ptr<Texture> m_sceneTexture, m_bloomTexture, m_brightTexture, m_blurTexture;

    GLuint m_attribute_vertex, m_attribute_texcoord;
    GLuint m_quad_vao, m_quad_vbo_vertices, m_quad_vbo_texcoords;

};
}  // namespace vr
