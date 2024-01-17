#include "ImGuiLayer.hpp"

ImGuiLayer::ImGuiLayer(GLFWwindow* window, Scene* sceneManager) {
    this->sceneManager = sceneManager;
    this->window = window;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    ImGui::StyleColorsDark();

    // Add a default font
    io.Fonts->AddFontDefault();
    // Load FontAwesome
    ImFontConfig config;
    config.MergeMode = true;
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromFileTTF("../resources/icons/fa-regular-400.ttf", 13.0f, &config, icon_ranges);
    io.Fonts->AddFontFromFileTTF("../resources/icons/fa-solid-900.ttf", 13.0f, &config, icon_ranges);
}

ImGuiLayer::~ImGuiLayer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::RenderUI() {
    // Makes the entire window a dockspace
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    RenderViewport();
}

void ImGuiLayer::RenderViewport() {
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNavInputs);
    viewportSelected = ImGui::IsWindowFocused();

    // Only rescale the framebuffer if the viewport size has changed
    ImVec2 newViewportSize = ImGui::GetWindowSize();
    if (newViewportSize.x != viewportSize.x || newViewportSize.y != viewportSize.y) {
        viewportSize = newViewportSize;
        // renderer->RescaleFramebuffer(viewportSize.x, viewportSize.y);
        glViewport(0, 0, viewportSize.x, viewportSize.y);
    }
    // ImGui::Image((void*)(intptr_t)renderer->ViewportTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImVec2 viewportPos = ImGui::GetWindowPos();
    ImGui::End();
}

void ImGuiLayer::OpenFileDialog(std::string key) {
    if (key == "Load Object") {
        if (fileDialog.Display(key)) {
            if (fileDialog.IsOk() == true) {
                loadedFile = fileDialog.GetCurrentFileName();
                loadedFilePath = fileDialog.GetCurrentPath();
                if (sceneManager->LoadObject(loadedFilePath + "/" + loadedFile) < 0) {
                    std::cout << "Failed to load " << loadedFilePath + "/" + loadedFile << std::endl;
                } else {
                    std::cout << "Loaded " << loadedFilePath + "/" + loadedFile << std::endl;
                }
            }
            fileDialog.Close();
        }
    } else if (key == "Add Object") {
        if (fileDialog.Display(key)) {
            if (fileDialog.IsOk() == true) {
                loadedFile = fileDialog.GetCurrentFileName();
                loadedFilePath = fileDialog.GetCurrentPath();
                if (sceneManager->AddObject(loadedFilePath + "/" + loadedFile) < 0) {
                    std::cout << "Failed to load " << loadedFilePath + "/" + loadedFile << std::endl;
                } else {
                    std::cout << "Loaded " << loadedFilePath + "/" + loadedFile << std::endl;
                }
            }
            fileDialog.Close();
        }
    }
}

void ImGuiLayer::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();

    // Forward key events to ImGui
    if (action == GLFW_PRESS) {
        io.KeysDown[key] = true;
    } else if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

    // Handle shortcuts
    if (io.KeyCtrl && io.KeysDown[GLFW_KEY_O]) {
        fileDialog.OpenDialog("Load Object", "Choose File", ".obj", "../resources/objects/");
    }
}

// I'm polling key events here because I want to keep the camera movement constant independent of framerate
void ImGuiLayer::PollKeyEvents() {
    ImGuiIO& io = ImGui::GetIO();

    // Keeps translation, rotation, camera speed constant independent of framerate
    float translationRate = TRANSLATION_RATE * io.DeltaTime;
    float rotationRate = ROTATION_RATE * io.DeltaTime;
    float cameraSpeed = CAMERA_SPEED * io.DeltaTime;

    // Translation, rotation, scale values
    glm::vec3 tVals = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rVals = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sVals = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 tValsCamera = glm::vec3(0.0f, 0.0f, 0.0f);

    if (viewportSelected) {
        // Translation
        tVals.x = HandleTransformInput(GLFW_KEY_L, GLFW_KEY_J, translationRate);
        tVals.y = HandleTransformInput(GLFW_KEY_I, GLFW_KEY_K, translationRate);
        tVals.z = HandleTransformInput(GLFW_KEY_U, GLFW_KEY_O, translationRate);

        // Rotation
        rVals.x = HandleTransformInput(GLFW_KEY_UP, GLFW_KEY_DOWN, rotationRate);
        rVals.y = HandleTransformInput(GLFW_KEY_RIGHT, GLFW_KEY_LEFT, rotationRate);
        rVals.z = HandleTransformInput(GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN, rotationRate);

        // Scale
        float scaleFactor = 1.0f + HandleTransformInput(GLFW_KEY_KP_ADD, GLFW_KEY_KP_SUBTRACT, SCALE_RATE);
        sVals = glm::vec3(scaleFactor, scaleFactor, scaleFactor);

        // Camera
        tValsCamera.x = HandleTransformInput(GLFW_KEY_D, GLFW_KEY_A, cameraSpeed);
        tValsCamera.y = HandleTransformInput(GLFW_KEY_E, GLFW_KEY_Q, cameraSpeed);
        tValsCamera.z = HandleTransformInput(GLFW_KEY_W, GLFW_KEY_S, cameraSpeed);

        // Apply transformations
        sceneManager->GetSelectedObject().Translate(tVals);
        sceneManager->GetSelectedObject().Rotate(rVals.x, glm::vec3(1.0f, 0.0f, 0.0f));
        sceneManager->GetSelectedObject().Rotate(rVals.y, glm::vec3(0.0f, 1.0f, 0.0f));
        sceneManager->GetSelectedObject().Rotate(rVals.z, glm::vec3(0.0f, 0.0f, 1.0f));
        sceneManager->GetSelectedObject().Scale(sVals);
        sceneManager->GetActiveCamera().Translate(tValsCamera);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        sceneManager->ResetScene();
        yaw = -90.0f;
        pitch = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float ImGuiLayer::HandleTransformInput(int positveKey, int negativeKey, float rate) {
    float value = 0.0f;
    if (glfwGetKey(window, positveKey) == GLFW_PRESS) {
        value += rate;
    }
    if (glfwGetKey(window, negativeKey) == GLFW_PRESS) {
        value -= rate;
    }
    return value;
}

void ImGuiLayer::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action);
}

void ImGuiLayer::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void ImGuiLayer::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xpos, (float)ypos);

    if (viewportSelected && io.MouseDown[0]) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glm::vec3 front;
        if (firstMouse) {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        // Calculate mouse offset since last frame
        float offsetX = xpos - lastMouseX;
        float offsetY = lastMouseY - ypos;  // Reversed since y-coordinates range from bottom to top
        lastMouseX = xpos;
        lastMouseY = ypos;

        // Apply mouse sensitivity
        offsetX *= MOUSE_SENSITIVITY;
        offsetY *= MOUSE_SENSITIVITY;

        // Apply yaw and pitch
        yaw += offsetX;
        pitch += offsetY;

        // Constrain pitch
        if (pitch > 89.0f)
            pitch = 89.0f;
        else if (pitch < -89.0f)
            pitch = -89.0f;

        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        sceneManager->GetActiveCamera().Rotate(front);

    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // Show cursor
        firstMouse = true;
    }
}
