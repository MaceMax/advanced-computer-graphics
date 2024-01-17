#include "Application.hpp"

Application::Application(std::string title, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    this->title = title;

    // Initialize GLFW
    InitGlfw();
    // Initialize Scene Manager
    scene = new Scene();

    // Initialize ImGui
    imGuiLayer = new ImGuiLayer(window, scene);

    // Set graphics attributes
    glPointSize(5.0);  // Unsupported in OpenGL ES 2.0
    glLineWidth(1.0);
    glClearColor(0.2, 0.2, 0.2, 0.0);
    glViewport(0, 0, width, height);

    // Set callbacks
    callbackManager->init(window);
    callbackManager->SetResizeCallback(std::bind(&Application::ResizeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    callbackManager->SetErrorCallback(std::bind(&Application::ErrorCallback, this, std::placeholders::_1, std::placeholders::_2));
    callbackManager->SubscribeToKeyEvents(std::bind(&ImGuiLayer::KeyCallback, imGuiLayer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    callbackManager->SubscribeToKeyEvents(std::bind(&Application::KeyCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    callbackManager->SubscribeToMouseButtonEvents(std::bind(&ImGuiLayer::MouseButtonCallback, imGuiLayer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    callbackManager->SubscribeToMousePositionEvents(std::bind(&ImGuiLayer::MouseCallback, imGuiLayer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    callbackManager->SubscribeToScrollEvents(std::bind(&ImGuiLayer::ScrollCallback, imGuiLayer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::InitGlfw() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set minimum supported OpenGL version and OpenGL profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        std::cerr << "Could not open window or initialize OpenGL context." << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize glew
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cout << "glew init error:" << std::endl
                  << glewGetErrorString(glewError) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!GLEW_VERSION_4_3) {
        std::cout << "Warning: OpenGL 4.3+ not supported by the GPU!" << std::endl;
        std::cout << "Decreace supported OpenGL version if needed." << std::endl;
    }
}

GLFWwindow* Application::Window() const {
    return window;
}

int Application::Width() const {
    return windowWidth;
}

int Application::Height() const {
    return windowHeight;
}

void Application::ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
    exit(EXIT_FAILURE);
}

void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

void Application::ResizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void Application::Run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        imGuiLayer->RenderUI();
        // Polling because callbacks are not smooth
        imGuiLayer->PollKeyEvents();
        ImGui::Render();

        renderer->Render();
        CheckOpenGLError();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}
