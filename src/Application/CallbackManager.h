#pragma once
#include "3dStudio.hpp"

/**
 * CallbackManager class. This class is responsible for handling the callbacks
 * from GLFW. It is initialized in the Application class. Allows classes to
 * subscribe to the callbacks.
 */
class CallbackManager {
   public:
    /**
     * Callback function types. Makes it easier to subscribe to the callbacks.
     */
    using KeyCallback = std::function<void(GLFWwindow*, int, int, int, int)>;
    using MousePositionCallback = std::function<void(GLFWwindow*, double, double)>;
    using MouseButtonCallback = std::function<void(GLFWwindow*, int, int, int)>;
    using ScrollCallback = std::function<void(GLFWwindow*, double, double)>;
    using ResizeCallback = std::function<void(GLFWwindow*, int, int)>;
    using ErrorCallback = std::function<void(int, const char*)>;

    static void init(GLFWwindow* window);

    /**
     * @brief Set the Resize Callback function. Cannot be subscribed to.
     *
     * @param callback The callback function
     */
    void SetResizeCallback(ResizeCallback callback);

    /**
     * @brief Set the Error Callback function. Cannot be subscribed to.
     *
     * @param callback The callback function
     */
    void SetErrorCallback(ErrorCallback callback);

    /**
     * @brief Subscribe to mouse button events.
     *
     * @param callback The callback function
     */
    void SubscribeToMouseButtonEvents(MouseButtonCallback callback);

    /**
     * @brief Subscribe to scroll events.
     *
     * @param callback The callback function
     */
    void SubscribeToScrollEvents(ScrollCallback callback);

    /**
     * @brief Subscribe to mouse position events.
     *
     * @param callback The callback function
     */
    void SubscribeToMousePositionEvents(MousePositionCallback callback);

    /**
     * @brief Subscribe to key events.
     *
     * @param callback The callback function
     */
    void SubscribeToKeyEvents(KeyCallback callback);

   private:
    static ResizeCallback resizeCallback;
    static ErrorCallback errorCallback;

    /**
     * Wrapper functions for the callbacks. Calls all the subscribed callbacks.
     */
    static void MousePositionbackWrapper(GLFWwindow* appWindow, double xpos, double ypos);
    static void MouseButtonCallbackWrapper(GLFWwindow* appWindow, int button, int action, int mods);
    static void ScrollCallbackWrapper(GLFWwindow* appWindow, double xoffset, double yoffset);
    static void KeyCallbackWrapper(GLFWwindow* appWindow, int key, int scancode, int action, int mods);
    static void ErrorCallbackWrapper(int error, const char* description);
    static void ResizeCallbackWrapper(GLFWwindow* appWindow, int width, int height);

    /**
     * Callback vectors. Stores the subscribed callbacks.
     */
    static std::vector<MouseButtonCallback> mouseButtonCallbacks;
    static std::vector<ScrollCallback> scrollCallbacks;
    static std::vector<MousePositionCallback> mousePositionCallbacks;
    static std::vector<KeyCallback> keyCallbacks;
};