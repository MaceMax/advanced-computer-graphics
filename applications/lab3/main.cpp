#ifndef _WIN32
/*
Workaround for some kind of linker bug. See https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
*/
#include <pthread.h>
void junk() {
    int i;
    i = pthread_getconcurrency();
};
#endif

#ifdef _WIN32
#include <windows.h>
#endif

// Important: Import glad as first gl header
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vr/Application.h>
#include <vr/Version.h>
#include <vr/glErrorUtil.h>

#include <glm/vec2.hpp>
#include <iostream>
#include <sstream>
#include <thread>

#define FPS_LIMIT 144.0

// Weak pointer to the application, so we can access it in callbacks
std::weak_ptr<vr::Application> g_applicationPtr;

void scroll_mouse_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (auto app = g_applicationPtr.lock()) {
        float fov = app->getCamera()->getFOV();
        fov += (float)yoffset;
        app->getCamera()->setFOV(fov);
    }
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    int shouldClose = 0;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        if (auto app = g_applicationPtr.lock()) {
            app->reloadScene();
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            app->getCamera()->setScreenSize(glm::uvec2(width, height));
        }
            
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        if (auto app = g_applicationPtr.lock())
            app->initView();

    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        if (auto app = g_applicationPtr.lock())
            app->toggleShadows();

    // Select lights with 0-9 keys
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock()) {
            int lightIdx = key - GLFW_KEY_0;
            app->selectLight(lightIdx);
        }
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock())
            app->changeCamera(1);
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock())
            app->changeCamera(-1);
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock())
            app->toogleDebug();
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock())
            app->changeDebugTexture(1);
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock())
            app->changeDebugTexture(-1);
    }

    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock())
            app->toggleBloom();
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if (auto app = g_applicationPtr.lock())
            app->toggleDOF();
    }

}

void window_size_callback(GLFWwindow* window, int width, int height) {
    if (auto app = g_applicationPtr.lock())
        app->setScreenSize(width, height);
}

GLFWwindow* initializeWindows(int width, int height) {
    GLFWwindow* window = nullptr;

    // Initialize GLFW
    if (!glfwInit())
        return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "ObjViewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyboard_callback);

    glfwSetScrollCallback(window, scroll_mouse_callback);

    glfwSetWindowSizeCallback(window, window_size_callback);

    glfwSwapInterval(0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        std::cerr << "Error: " << gl_error_string(glGetError()) << std::endl;
        glfwTerminate();
        glfwDestroyWindow(window);
        return nullptr;
    }

    printf("GL version: %s\n", glGetString(GL_VERSION));
    printf("GL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return window;
}

void cleanupWindows(GLFWwindow* window) {
    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
}

int main(int argc, char** argv) {
    const unsigned SCREEN_WIDTH = 1920;
    const unsigned SCREEN_HEIGHT = 1080;

    std::cerr << vr::getName() << ": " << vr::getVersion() << std::endl
              << std::endl;

    GLFWwindow* window = initializeWindows(SCREEN_WIDTH, SCREEN_HEIGHT);

    std::shared_ptr<vr::Application> application = std::make_shared<vr::Application>(SCREEN_WIDTH, SCREEN_HEIGHT);
    g_applicationPtr = application;

    std::string model_filename;
    if (argc > 1)
        model_filename = argv[1];

    std::string v_shader_filename = "shaders/gbuffer.vs";
    std::string f_shader_filename = "shaders/gbuffer.fs";

    if (argc < 2) {
        std::cerr << "Loading a default box built by \"hand\" " << std::endl;
        std::cerr << "\n\nUsage: " << argv[0] << " <model-file>" << std::endl;
    }

    if (!application->initResources(model_filename, v_shader_filename, f_shader_filename)) {
        cleanupWindows(window);
        std::cerr << "Failed to load resources" << std::endl;
        return 1;
    }

    application->getCamera()->setFOV(60);
    application->initView();

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_FRAMEBUFFER_SRGB);
   
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        application->update(window);

        glfwSwapBuffers(window);
        glfwPollEvents();

        application->processInput(window);

        // Limit the frame rate to 144 FPS
        while (glfwGetTime() - lastTime < 1.0 / FPS_LIMIT) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        lastTime += 1.0 / FPS_LIMIT;

    }

    // Shutdown the application BEFORE we loose OpenGL Context
    application = nullptr;

    cleanupWindows(window);

    return 0;
}
