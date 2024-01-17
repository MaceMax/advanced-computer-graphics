#include <imgui_internal.h>

#include "3dStudio.hpp"
#include "IconsFontAwesome6.h"
#include "Scene.hpp"
#define MOUSE_SENSITIVITY 0.1f
#define CAMERA_SPEED 1.0f
#define TRANSLATION_RATE 1.0f
#define ROTATION_RATE 1.0f
#define SCALE_RATE 0.01f

/**
 * ImGuiLayer class. This class is responsible for rendering the ImGui UI.
 * It also handles the input events from the user. It is initialized in the
 * Application class. Holds a pointer to the Scene and the Renderer,
 * so it can modify the scene and the renderer. Not best practice, but it
 * works.
 */
class ImGuiLayer {
   public:
    ImGuiLayer(GLFWwindow* window, Scene* scene);
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
    Scene* sceneManager;
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

    // Camera variables
    float fovSlider = 45.0f;
    float nearPlaneSlider = 0.1f;
    float farPlaneSlider = 100.0f;
    float topSlider = 1.0f;
    float obliqueAngleSlider = 0.0f;
    float obliqueScaleSlider = 0.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;

    /**
     * Renders the framebuffer texture to the viewport.
     */
    void RenderViewport();

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
};
