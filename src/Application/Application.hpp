#pragma once

#include "3dStudio.hpp"
#include "CallbackManager.h"
#include "ImGuiLayer.hpp"
#include "Scene.hpp"

/**
 * Application class. This class is responsible for the main loop of the
 * application. It handles the window and the callbacks. It also
 * initializes the ImGuiLayer, the Scene and the
 * CallbackManager.
 */

class Application {
   public:
    Application(std::string title, int width, int height);
    ~Application();

    /**
     * Main loop of the application. It calls the Render methods
     * of the Renderer and the ImGuiLayer. It also swaps the buffers
     * and polls the events.
     */
    void Run();
    /**
     * Callback for the error. It prints the error and the description
     * of the error.
     *
     * @param error The error code.
     * @param description The description of the error.
     */
    void ErrorCallback(int error, const char* description);
    /**
     * Callback for the window resize. It calls the Reshape method.
     *
     * @param window The window.
     * @param width The new width of the window.
     * @param height The new height of the window.
     */
    void ResizeCallback(GLFWwindow* window, int width, int height);
    /**
     * Callback for the keyboard. It calls the KeyCallback method of the
     * CallbackManager.
     *
     * @param window The window.
     * @param key The key that was pressed or released.
     * @param scancode The system-specific scancode of the key.
     * @param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
     * @param mods Bit field describing which modifier keys were held down.
     */
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    /**
     * Returns the GLFWwindow.
     *
     * @return The window.
     */
    GLFWwindow* Window() const;
    /**
     * Returns the width of the window.
     *
     * @return The width of the window.
     */
    int Width() const;
    /**
     * Returns the height of the window.
     *
     * @return The height of the window.
     */
    int Height() const;

   private:
    GLFWwindow* window;
    ImGuiLayer* imGuiLayer;
    Scene* scene;
    CallbackManager* callbackManager;

    int windowWidth;
    int windowHeight;
    std::string title;

    /**
     * Initializes the GLFW library.
     */
    void InitGlfw();
};