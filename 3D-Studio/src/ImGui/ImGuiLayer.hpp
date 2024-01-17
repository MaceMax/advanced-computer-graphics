#include <imgui_internal.h>

#include "3dStudio.hpp"
#include "IconsFontAwesome6.h"
#include "MaterialManager.hpp"
#include "Renderer.hpp"
#include "SceneManager.hpp"
#define MOUSE_SENSITIVITY 0.1f
#define CAMERA_SPEED 1.0f
#define TRANSLATION_RATE 1.0f
#define ROTATION_RATE 1.0f
#define SCALE_RATE 0.01f

/**
 * ImGuiLayer class. This class is responsible for rendering the ImGui UI.
 * It also handles the input events from the user. It is initialized in the
 * Application class. Holds a pointer to the SceneManager and the Renderer,
 * so it can modify the scene and the renderer. Not best practice, but it
 * works.
 */
class ImGuiLayer {
   public:
    ImGuiLayer(GLFWwindow* window, SceneManager* sceneManager, Renderer* renderer);
    ~ImGuiLayer();

    /**
     * Renders the ImGui UI. Gets called in the main loop of the application.
     */
    void RenderUI();

    /**
     * Polls the key events. Gets called in the main loop of the application.
     */
    void PollKeyEvents();
    /**
     * Key callback. Gets called when a key is pressed or released. Used to forward
     * the key events to ImGui. Is also used for shortcuts.
     *
     * @param window GLFWwindow pointer
     * @param key Key code
     * @param scancode Scancode
     * @param action Action
     * @param mods Mods
     */

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    /**
     * Mouse button callback. Gets called when a mouse button is pressed or released.
     * Used to forward the mouse button events to ImGui.
     *
     * @param window GLFWwindow pointer
     * @param button Button code
     * @param action Action
     * @param mods Mods
     */

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    /**
     * Mouse callback. Gets called when the mouse is moved. Used to forward the mouse
     * movement events to ImGui. Also used to rotate the camera.
     *
     * @param window GLFWwindow pointer
     * @param xpos X position
     * @param ypos Y position
     */
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);

    /**
     * Scroll callback. Gets called when the mouse wheel is scrolled. Used to forward
     * the scroll events to ImGui.
     *
     * @param window GLFWwindow pointer
     * @param xoffset X offset
     * @param yoffset Y offset
     */
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

   private:
    GLFWwindow* window;
    SceneManager* sceneManager;
    Renderer* renderer;
    MaterialManager& materialManager = MaterialManager::getInstance();
    ImVec2 viewportSize = ImVec2(0, 0);
    ImGuiFileDialog fileDialog;
    ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;

    // ImGui variables
    std::string loadedFile = "";
    std::string loadedFilePath = "";
    bool viewportSelected = false;
    bool firstMouse = true;
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    int selectedMeshIndex = 0;
    int selectedSubMeshIndex = 0;
    int selectedType = 0;
    int selectedLightIndex = 0;

    // Camera variables
    float fovSlider = 45.0f;
    float nearPlaneSlider = 0.1f;
    float farPlaneSlider = 100.0f;
    float topSlider = 1.0f;
    float obliqueAngleSlider = 0.0f;
    float obliqueScaleSlider = 0.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Default material variables
    glm::vec3 defaultAmbientSlider;
    glm::vec3 defaultDiffuseSlider;
    glm::vec3 defaultSpecularSlider;
    float defaultShininessSlider;

    // Available textures
    std::vector<std::string> availableTextures;

    /**
     * Renders the framebuffer texture to the viewport.
     */
    void RenderViewport();

    /**
     * Renders the selected's mesh properties.
     *
     * @param pos The position of the viewport.
     */
    void RenderSelectedProperties(ImVec2 pos);

    /**
     * Renders the scene hierarchy.
     */
    void RenderSceneHierarchy();

    /**
     * Renders the scene properties.
     */
    void RenderProperties();

    /**
     * Renders the debug window.
     */
    void RenderDebug();

    /**
     * Renders the object details window.
     */
    void RenderDetails();

    /**
     * Renders the light details window.
     */
    void RenderLightDetails();

    /**
     * Renders the menu bar.
     */
    void RenderMenuBar();

    /**
     * Handles transform input. Used for translation, rotation and scaling.
     * Checks if the positive or negative key is pressed and returns the
     * corresponding value.
     *
     * @param positveKey Positive key code
     * @param negativeKey Negative key code
     * @param rate Rate of the transformation (speed)
     * @return float The value of the transformation
     */
    float HandleTransformInput(int positveKey, int negativeKey, float rate);

    /**
     * Opens the file dialog.
     *
     * @param key The key of the file dialog
     */
    void OpenFileDialog(std::string key);

    /**
     * Updates the available textures.
     */
    void UpdateAvailableTextures();
};
