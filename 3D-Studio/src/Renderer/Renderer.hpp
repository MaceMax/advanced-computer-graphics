#pragma once
#include "3dStudio.hpp"
#include "SceneManager.hpp"

/**
 * Renderer class. This class is responsible for rendering the scene. It is
 * initialized in the Application class. Holds a pointer to the SceneManager,
 * so it can fetch the scene data.
 */
class Renderer {
   public:
    Renderer(SceneManager* sceneManager);
    ~Renderer();

    /**
     * Renders the scene.
     */
    void Render();

    /**
     * Changes the shader program.
     *
     * @param shadingModel The shader program to use.
     */
    void ChangeShaderProgram(ShaderProgram shadingModel);

    /**
     * Changes the face culling mode.
     *
     * @param showBackface True if backface should be shown, false otherwise
     */
    void ShowBackface(bool showBackface);

    /**
     * Changes the skybox rendering mode.
     *
     * @param showSkybox True if skybox should be shown, false otherwise
     */
    void ShowSkybox(bool showSkybox);

    /**
     * Changes the world plane rendering mode.
     *
     * @param showWorldPlane True if world plane should be shown, false otherwise
     */
    void ShowWorldPlane(bool showWorldPlane);

    /**
     * @brief Set the Toon Levels Uniform for the toon shader.
     *
     * @param toonLevels The number of toon levels
     */
    void SetToonLevels(int toonLevels);

    /**
     * Rescales the framebuffer. Is called when the viewport is resized.
     *
     * @param width The new width
     * @param height The new height
     */
    void RescaleFramebuffer(float width, float height);

    /**
     * @brief Get the Viewport Texture ID
     *
     * @return GLuint The viewport texture ID
     */
    GLuint ViewportTexture() const;

   private:
    std::vector<GLuint> shaderPrograms;
    GLuint shaderProgram;
    GLuint locM;
    GLuint locV;
    GLuint locP;
    GLuint locVertices;
    GLuint locNormals;
    GLuint locTexCoords;

    // Buffers
    GLuint fbo;
    GLuint rbo;
    GLuint viewportTexture;

    bool renderSkybox = true;
    bool renderWorldPlane = true;

    SceneManager* sceneManager;
    void RenderScene();
    void RenderLights();
    void RenderSkybox();
    void RenderWorldPlane();

    /**
     * Reads the shader source from a file.
     *
     * @param shaderFile  The shader file
     * @return std::string The shader source
     */
    std::string ReadShaderSource(const std::string shaderFile);

    /**
     * Creates a shader program from a vertex and fragment shader.
     *
     * @param vShaderFile The vertex shader file
     * @param fShaderFile The fragment shader file
     * @return GLuint The shader program ID
     */
    GLuint CreateShader(const std::string vShaderFile, const std::string fShaderFile);
};
