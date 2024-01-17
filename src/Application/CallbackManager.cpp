#include "CallbackManager.h"

std::vector<CallbackManager::KeyCallback> CallbackManager::keyCallbacks;
std::vector<CallbackManager::MouseButtonCallback> CallbackManager::mouseButtonCallbacks;
std::vector<CallbackManager::ScrollCallback> CallbackManager::scrollCallbacks;
std::vector<CallbackManager::MousePositionCallback> CallbackManager::mousePositionCallbacks;
CallbackManager::ResizeCallback CallbackManager::resizeCallback;
CallbackManager::ErrorCallback CallbackManager::errorCallback;

void CallbackManager::init(GLFWwindow* appWindow) {
    glfwSetErrorCallback(ErrorCallbackWrapper);
    glfwSetFramebufferSizeCallback(appWindow, ResizeCallbackWrapper);
    glfwSetKeyCallback(appWindow, KeyCallbackWrapper);
    glfwSetMouseButtonCallback(appWindow, MouseButtonCallbackWrapper);
    glfwSetScrollCallback(appWindow, ScrollCallbackWrapper);
    glfwSetCursorPosCallback(appWindow, MousePositionbackWrapper);
}

void CallbackManager::ErrorCallbackWrapper(int error, const char* description) {
    errorCallback(error, description);
}

void CallbackManager::ResizeCallbackWrapper(GLFWwindow* appWindow, int width, int height) {
    resizeCallback(appWindow, width, height);
}

void CallbackManager::KeyCallbackWrapper(GLFWwindow* appWindow, int key, int scancode, int action, int mods) {
    for (auto& callback : keyCallbacks) {
        callback(appWindow, key, scancode, action, mods);
    }
}

void CallbackManager::MouseButtonCallbackWrapper(GLFWwindow* appWindow, int button, int action, int mods) {
    for (auto& callback : mouseButtonCallbacks) {
        callback(appWindow, button, action, mods);
    }
}

void CallbackManager::ScrollCallbackWrapper(GLFWwindow* appWindow, double xoffset, double yoffset) {
    for (auto& callback : scrollCallbacks) {
        callback(appWindow, xoffset, yoffset);
    }
}

void CallbackManager::MousePositionbackWrapper(GLFWwindow* appWindow, double xpos, double ypos) {
    for (auto& callback : mousePositionCallbacks) {
        callback(appWindow, xpos, ypos);
    }
}

void CallbackManager::SetResizeCallback(ResizeCallback callback) {
    resizeCallback = callback;
}

void CallbackManager::SetErrorCallback(ErrorCallback callback) {
    errorCallback = callback;
}

void CallbackManager::SubscribeToKeyEvents(KeyCallback callback) {
    keyCallbacks.push_back(callback);
}

void CallbackManager::SubscribeToMouseButtonEvents(MouseButtonCallback callback) {
    mouseButtonCallbacks.push_back(callback);
}

void CallbackManager::SubscribeToScrollEvents(ScrollCallback callback) {
    scrollCallbacks.push_back(callback);
}

void CallbackManager::SubscribeToMousePositionEvents(MousePositionCallback callback) {
    mousePositionCallbacks.push_back(callback);
}
