#include "ImGuiLayer.hpp"

ImGuiLayer::ImGuiLayer(GLFWwindow* window, SceneManager* sceneManager, Renderer* renderer) {
    this->sceneManager = sceneManager;
    this->renderer = renderer;
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

    defaultAmbientSlider = materialManager.GetDefaultMaterial().ambient;
    defaultDiffuseSlider = materialManager.GetDefaultMaterial().diffuse;
    defaultSpecularSlider = materialManager.GetDefaultMaterial().specular;
    defaultShininessSlider = materialManager.GetDefaultMaterial().shininess;
    UpdateAvailableTextures();

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
    RenderMenuBar();
    RenderViewport();
    RenderDebug();
    RenderSceneHierarchy();
    RenderProperties();
}

void ImGuiLayer::RenderViewport() {
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNavInputs);
    viewportSelected = ImGui::IsWindowFocused();

    // Only rescale the framebuffer if the viewport size has changed
    ImVec2 newViewportSize = ImGui::GetWindowSize();
    if (newViewportSize.x != viewportSize.x || newViewportSize.y != viewportSize.y) {
        viewportSize = newViewportSize;
        renderer->RescaleFramebuffer(viewportSize.x, viewportSize.y);
        glViewport(0, 0, viewportSize.x, viewportSize.y);
    }
    ImGui::Image((void*)(intptr_t)renderer->ViewportTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImVec2 viewportPos = ImGui::GetWindowPos();
    ImGui::End();

    // Render selected object's properties
    RenderSelectedProperties(viewportPos);
}

void ImGuiLayer::RenderSelectedProperties(ImVec2 viewportPos) {
    // Position it in the top left corner of the viewport
    ImGui::SetNextWindowPos(ImVec2(viewportPos.x + 25, viewportPos.y + 30));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking |
                                    ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowBgAlpha(0.55f);  // Transparent background

    switch (selectedType) {
        case 0:  // Mesh
        {
            int index = selectedMeshIndex != -1 ? 0 : selectedSubMeshIndex;
            Mesh mesh = sceneManager->GetSelectedObject();
            SubMesh subMesh = sceneManager->GetSelectedObject().GetSubMesh(index);
            ImGui::Begin(mesh.GetFilename().c_str(), nullptr, window_flags);
            ImGui::SeparatorText("Object information");
            ImGui::Text("File: %s", mesh.GetFilename().c_str());
            ImGui::Text("Directory: %s", mesh.GetDirectory().c_str());
            ImGui::Text("Total Vertices: %d", mesh.GetVertexCount());
            ImGui::Text("Faces: %d", mesh.GetFaceCount());

            if (ImGui::Button(ICON_FA_SYNC)) {
                UpdateAvailableTextures();
            }
            ImGui::SameLine();

            if (ImGui::BeginCombo("Texture", mesh.GetTextureName().c_str())) {
                for (size_t i = 0; i < availableTextures.size(); i++) {
                    bool isSelected = (availableTextures[i] == mesh.GetTextureName());
                    if (ImGui::Selectable(availableTextures[i].c_str(), isSelected)) {
                        sceneManager->GetSelectedObject().LoadTexture("../resources/textures/" + availableTextures[i], sceneManager->GetSelectedObject().GetMainTexture());
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            bool showTexture = mesh.IsShowingTexture();
            if (ImGui::Checkbox("Show texture", &showTexture)) {
                sceneManager->GetSelectedObject().ShowTexture(showTexture);
            }

            if (mesh.GetSubMeshCount() > 1 && selectedSubMeshIndex != -1) {
                std::string subMeshName = "Submesh: " + subMesh.GetName();
                ImGui::SeparatorText(subMeshName.c_str());
                ImGui::Text("Vertices: %d", subMesh.GetVertexCount());
                ImGui::Text("Faces: %d", subMesh.GetFaceCount());
            }

            if (selectedSubMeshIndex == -1) {
                std::unordered_map<GLint, Material> meshMaterials = materialManager.GetMeshMaterials(mesh.GetName());
                ImGui::SeparatorText("Mesh materials");
                for (auto const& pair : meshMaterials) {
                    std::string materialName = "Material #" + std::to_string(pair.first);
                    if (ImGui::TreeNode(materialName.c_str(), pair.second.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                        // Display material properties, not editable
                        if (pair.second.ambientMapPath != "") {
                            ImGui::Text("Ambient map: %s", pair.second.ambientMapPath.c_str());
                        } else {
                            ImGui::ColorEdit3("Ambient reflection constant", (float*)&pair.second.ambient, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
                        }

                        if (pair.second.diffuseMapPath != "") {
                            ImGui::Text("Diffuse map: %s", pair.second.diffuseMapPath.c_str());
                        } else {
                            ImGui::ColorEdit3("Diffuse reflection constant", (float*)&pair.second.diffuse, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
                        }

                        if (pair.second.specularMapPath != "") {
                            ImGui::Text("Specular map: %s", pair.second.specularMapPath.c_str());
                        } else {
                            ImGui::ColorEdit3("Specular reflection constant", (float*)&pair.second.specular, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
                        }
                        ImGui::Text("Shininess factor: %.2f", pair.second.shininess);
                        ImGui::TreePop();
                    }
                }
            }

            ImGui::End();

        } break;
        case 1:
            ImGui::Begin("Light properties", nullptr, window_flags);
            RenderLightDetails();
            ImGui::End();
            break;

        default:
            break;
    }
}

void ImGuiLayer::RenderSceneHierarchy() {
    ImGuiTreeNodeFlags nodeFlags;

    ImGui::Begin("Scene Hierarchy");

    ImGui::SeparatorText("Information");
    ImGui::TextWrapped(
        "To add an object, click on the \"Add Object\" button. This will open a file dialog. Select an object file (.obj) and click \"Open\"."
        "The object will be added to the scene. To clear the scene of objects, you can add an object via the File menu tab (CTRL+O), which will clear the scene and add the new object."
        "To select an object, click on it in the scene hierarchy. An object can have multiple submeshes."
        "An object can only be transformed when it is selected in the scene hierarchy and the viewport is in focus.");

    ImGui::SeparatorText("Scene");
    ImGui::Text("Objects");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PLUS " Add Object")) {
        fileDialog.OpenDialog("Add Object", "Choose File", ".obj", "../resources/objects/");
    }

    for (size_t i = 0; i < sceneManager->GetSceneObjects().size(); i++) {
        Mesh mesh = sceneManager->GetSceneObjects()[i];
        std::vector<std::string> meshNames = mesh.GetSubMeshNames();
        // If we only have one mesh, don't show the tree node, show a leaf instead
        nodeFlags = meshNames.size() == 1 ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;
        if ((int)i == selectedMeshIndex) {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }
        if (ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", mesh.GetName().c_str())) {
            if (ImGui::IsItemClicked()) {
                selectedMeshIndex = i;
                selectedSubMeshIndex = -1;
                selectedType = 0;
                sceneManager->SelectObject(i);
            }

            for (size_t j = meshNames.size() == 1 ? 1 : 0; j < meshNames.size(); j++) {
                nodeFlags |= ImGuiTreeNodeFlags_Leaf;
                if ((int)j == selectedSubMeshIndex || (int)i == selectedMeshIndex) {
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                } else {
                    nodeFlags &= ~ImGuiTreeNodeFlags_Selected;
                }
                ImGui::TreeNodeEx((void*)(intptr_t)j, nodeFlags, "%s", meshNames[j].c_str());
                if (ImGui::IsItemClicked()) {
                    selectedSubMeshIndex = j;
                    selectedMeshIndex = -1;
                    selectedType = 0;
                    sceneManager->SelectObject(i);
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    ImGui::SeparatorText("Lights");
    nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    for (size_t i = 0; i < sceneManager->GetSceneLights().size(); i++) {
        if (ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "Light #%ld", i)) {
            if (ImGui::IsItemClicked()) {
                selectedLightIndex = i;
                selectedType = 1;
                sceneManager->SelectLight(i);
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
    OpenFileDialog("Add Object");
}

void ImGuiLayer::RenderProperties() {
    ImGui::Begin("Properties");
    RenderDetails();
    ImGui::End();

    ImGui::Begin("Global properties");
    static int proj_current_idx = 0;
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Control camera projection type, field of view, and far plane, near plane with sliders.
        const char* projectionTypes[] = {"Perspective", "Parallel"};

        ImGui::Combo("Projection", &proj_current_idx, projectionTypes, IM_ARRAYSIZE(projectionTypes), IM_ARRAYSIZE(projectionTypes));
        if (proj_current_idx == 0) {
            ImGui::SliderFloat("Field of view", &fovSlider, 20.0f, 160.0f, "%1.0f", flags);
            ImGui::SliderFloat("Far", &farPlaneSlider, 1.0f, 1000.0f, "%1.0f", flags);
            ImGui::SliderFloat("Near", &nearPlaneSlider, 0.01f, 1.0f, "%1.2f", flags);
        } else if (proj_current_idx == 1) {
            ImGui::SliderFloat("Top", &topSlider, 1.0f, 100.0f, "%.1f", flags);
            ImGui::SliderFloat("Far", &farPlaneSlider, 1.0f, 1000.0f, "%1.0f", flags);
            ImGui::SliderFloat("Near", &nearPlaneSlider, 0.01f, 1.0f, "%1.2f", flags);
            ImGui::SliderFloat("Oblique scale", &obliqueScaleSlider, 0.0f, 1.0f, "%.1f", flags);
            ImGui::SliderAngle("Oblique angle", &obliqueAngleSlider, -75, 75, "%1.0f", flags);
        }
    }

    if (proj_current_idx == 0) {
        sceneManager->GetActiveCamera().ProjectPerspective(fovSlider,
                                                           viewportSize.x / viewportSize.y,
                                                           nearPlaneSlider,
                                                           farPlaneSlider);
    } else if (proj_current_idx == 1) {
        sceneManager->GetActiveCamera().ProjectOrthographic(topSlider,
                                                            nearPlaneSlider,
                                                            farPlaneSlider,
                                                            obliqueScaleSlider,
                                                            obliqueAngleSlider, viewportSize.x / viewportSize.y);
    }

    if (ImGui::CollapsingHeader("Default material", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::ColorEdit3("Ambient reflection constant", (float*)&defaultAmbientSlider, ImGuiColorEditFlags_Float)) {
            materialManager.SetDefaultAmbient(defaultAmbientSlider);
        }
        if (ImGui::ColorEdit3("Diffuse reflection constant", (float*)&defaultDiffuseSlider, ImGuiColorEditFlags_Float)) {
            materialManager.SetDefaultDiffuse(defaultDiffuseSlider);
        }
        if (ImGui::ColorEdit3("Specular reflection constant", (float*)&defaultSpecularSlider, ImGuiColorEditFlags_Float)) {
            materialManager.SetDefaultSpecular(defaultSpecularSlider);
        }

        if (ImGui::SliderFloat("Shininess factor", &defaultShininessSlider, 0.0f, 10000.0f, "%0.1f", flags)) {
            materialManager.SetDefaultShininess(defaultShininessSlider);
        }
    }

    if (ImGui::CollapsingHeader("Shading", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* shadingTypes[] = {"Flat", "Gouraud", "Phong", "Wireframe", "Toon"};
        static int shading_current_idx = 2;
        static bool showBackface = true;
        if (ImGui::Combo("Active shading model", &shading_current_idx, shadingTypes, IM_ARRAYSIZE(shadingTypes), IM_ARRAYSIZE(shadingTypes))) {
            if (shading_current_idx == 0) {
                // renderer->ChangeShaderProgram(ShadingModel::FLAT);
            } else if (shading_current_idx == 1) {
                renderer->ChangeShaderProgram(ShaderProgram::GOURAUD);
                showBackface = true;
            } else if (shading_current_idx == 2) {
                renderer->ChangeShaderProgram(ShaderProgram::PHONG);
                showBackface = true;
            } else if (shading_current_idx == 3) {
                renderer->ChangeShaderProgram(ShaderProgram::WIREFRAME);
            } else if (shading_current_idx == 4) {
                showBackface = true;
                renderer->ChangeShaderProgram(ShaderProgram::TOON);
            }
        }

        if (shading_current_idx == 3) {
            if (ImGui::Checkbox("Show backface", &showBackface)) {
                renderer->ShowBackface(showBackface);
            }
        }

        if (shading_current_idx == 4) {
            static int toonLevels = 2;
            ImGui::SliderInt("Toon levels", &toonLevels, 1, 10);
            renderer->SetToonLevels(toonLevels);
        }
    }

    ImGui::SeparatorText("Skybox");
    static bool showSkybox = true;
    if (ImGui::Checkbox("Show skybox", &showSkybox)) {
        renderer->ShowSkybox(showSkybox);
    }

    ImGui::End();
}

void ImGuiLayer::RenderDetails() {
    if (selectedType == 0) {
        // Object details
    } else if (selectedType == 1) {
        // Light details
        RenderLightDetails();
    }
}

void ImGuiLayer::RenderLightDetails() {
    LightType lightType = sceneManager->GetSceneLights()[selectedLightIndex]->GetType();
    std::string lightName = "Light #" + std::to_string(selectedLightIndex);
    ImGui::SeparatorText(lightName.c_str());
    const char* lightTypes[] = {"Directional light", "Point light", "Spotlight"};
    static int light_current_idx = 0;
    if (ImGui::Combo("Caster type", &light_current_idx, lightTypes, IM_ARRAYSIZE(lightTypes), IM_ARRAYSIZE(lightTypes))) {
        if (light_current_idx == 0) {
            sceneManager->ChangeLightType(selectedLightIndex, LightType::DIRECTIONAL);
            lightType = LightType::DIRECTIONAL;
        } else if (light_current_idx == 1) {
            sceneManager->ChangeLightType(selectedLightIndex, LightType::POINT);
            lightType = LightType::POINT;
        } else if (light_current_idx == 2) {
            sceneManager->ChangeLightType(selectedLightIndex, LightType::SPOT);
            lightType = LightType::SPOT;
        }
    }

    if (lightType == LightType::DIRECTIONAL) {
        DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(sceneManager->GetSceneLights()[selectedLightIndex]);
        glm::vec3 direction = directionalLight->GetDirection();
        if (ImGui::SliderFloat3("Direction", (float*)&direction, -1.0f, 1.0f, "%.1f", flags)) {
            directionalLight->SetDirection(direction.x, direction.y, direction.z);
        }

        glm::vec3 ambientIntensity = directionalLight->GetAmbientIntensity();
        if (ImGui::ColorEdit3("Ambient Intensity", (float*)&ambientIntensity, ImGuiColorEditFlags_Float)) {
            directionalLight->SetAmbientIntensity(ambientIntensity.x, ambientIntensity.y, ambientIntensity.z);
        }

        glm::vec3 diffuseIntensity = directionalLight->GetDiffuseIntensity();
        if (ImGui::ColorEdit3("Diffuse intensity", (float*)&diffuseIntensity, ImGuiColorEditFlags_Float)) {
            directionalLight->SetDiffuseIntensity(diffuseIntensity.x, diffuseIntensity.y, diffuseIntensity.z);
        }

        glm::vec3 specularIntensity = directionalLight->GetSpecularIntensity();
        if (ImGui::ColorEdit3("Specular intensity", (float*)&specularIntensity, ImGuiColorEditFlags_Float)) {
            directionalLight->SetSpecularIntensity(specularIntensity.x, specularIntensity.y, specularIntensity.z);
        }

    } else if (lightType == LightType::POINT) {
        PointLight* pointLight = dynamic_cast<PointLight*>(sceneManager->GetSceneLights()[selectedLightIndex]);
        glm::vec3 position = pointLight->GetPosition();
        if (ImGui::SliderFloat3("Position", (float*)&position, -10.0f, 10.0f, "%.2f", flags)) {
            pointLight->SetPosition(position.x, position.y, position.z);
        }

        float constant = pointLight->GetConstant();
        if (ImGui::SliderFloat("Constant", &constant, 0.0f, 1.0f, "%.2f", flags)) {
            pointLight->SetConstant(constant);
        }

        float linear = pointLight->GetLinear();
        if (ImGui::SliderFloat("Linear", &linear, 0.0f, 1.0f, "%.2f", flags)) {
            pointLight->SetLinear(linear);
        }

        float quadratic = pointLight->GetQuadratic();
        if (ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.0f, "%.2f", flags)) {
            pointLight->SetQuadratic(quadratic);
        }

        glm::vec3 ambientIntensity = pointLight->GetAmbientIntensity();
        if (ImGui::ColorEdit3("Ambient Intensity", (float*)&ambientIntensity, ImGuiColorEditFlags_Float)) {
            pointLight->SetAmbientIntensity(ambientIntensity.x, ambientIntensity.y, ambientIntensity.z);
        }

        glm::vec3 diffuseIntensity = pointLight->GetDiffuseIntensity();
        if (ImGui::ColorEdit3("Diffuse intensity", (float*)&diffuseIntensity, ImGuiColorEditFlags_Float)) {
            pointLight->SetDiffuseIntensity(diffuseIntensity.x, diffuseIntensity.y, diffuseIntensity.z);
        }

        glm::vec3 specularIntensity = pointLight->GetSpecularIntensity();
        if (ImGui::ColorEdit3("Specular intensity", (float*)&specularIntensity, ImGuiColorEditFlags_Float)) {
            pointLight->SetSpecularIntensity(specularIntensity.x, specularIntensity.y, specularIntensity.z);
        }
    } else if (lightType == LightType::SPOT) {
        SpotLight* spotLight = dynamic_cast<SpotLight*>(sceneManager->GetSceneLights()[selectedLightIndex]);
        glm::vec3 position = spotLight->GetPosition();
        if (ImGui::SliderFloat3("Position", (float*)&position, -10.0f, 10.0f, "%.2f", flags)) {
            spotLight->SetPosition(position.x, position.y, position.z);
        }

        glm::vec3 direction = spotLight->GetDirection();
        if (ImGui::SliderFloat3("Direction", (float*)&direction, -1.0f, 1.0f, "%.1f", flags)) {
            spotLight->SetDirection(direction.x, direction.y, direction.z);
        }

        float cutoff = spotLight->GetCutoff();
        if (ImGui::SliderFloat("Cutoff", &cutoff, 0.0f, 90.0f, "%.1f", flags)) {
            spotLight->SetCutoff(cutoff);
        }

        float outerCutoff = spotLight->GetOuterCutoff();
        if (ImGui::SliderFloat("Outer cutoff", &outerCutoff, 0.0f, 90.0f, "%.1f", flags)) {
            spotLight->SetOuterCutoff(outerCutoff);
        }

        glm::vec3 ambientIntensity = spotLight->GetAmbientIntensity();
        if (ImGui::ColorEdit3("Ambient Intensity", (float*)&ambientIntensity, ImGuiColorEditFlags_Float)) {
            spotLight->SetAmbientIntensity(ambientIntensity.x, ambientIntensity.y, ambientIntensity.z);
        }

        glm::vec3 diffuseIntensity = spotLight->GetDiffuseIntensity();
        if (ImGui::ColorEdit3("Diffuse intensity", (float*)&diffuseIntensity, ImGuiColorEditFlags_Float)) {
            spotLight->SetDiffuseIntensity(diffuseIntensity.x, diffuseIntensity.y, diffuseIntensity.z);
        }

        glm::vec3 specularIntensity = spotLight->GetSpecularIntensity();
        if (ImGui::ColorEdit3("Specular intensity", (float*)&specularIntensity, ImGuiColorEditFlags_Float)) {
            spotLight->SetSpecularIntensity(specularIntensity.x, specularIntensity.y, specularIntensity.z);
        }
    }
}

void ImGuiLayer::RenderDebug() {
    ImGui::Begin("Debug");
    ImGui::BeginChild("Debug");
    ImGui::Text("Viewport selected: %s", viewportSelected ? "true" : "false");
    ImGui::Text("Viewport width: %f", viewportSize.x);
    ImGui::Text("Viewport height: %f", viewportSize.y);
    ImGui::EndChild();
    ImGui::End();
}

void ImGuiLayer::RenderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                // Open file dialog
                fileDialog.OpenDialog("Load Object", "Choose File", ".obj", "../resources/objects/");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    OpenFileDialog("Load Object");
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
                    selectedMeshIndex = sceneManager->GetSceneObjects().size() - 1;
                    selectedSubMeshIndex = -1;
                    sceneManager->SelectObject(selectedMeshIndex);
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
                    selectedMeshIndex = sceneManager->GetSceneObjects().size() - 1;
                    selectedSubMeshIndex = -1;
                    sceneManager->SelectObject(selectedMeshIndex);
                }
            }
            fileDialog.Close();
        }
    }
}

void ImGuiLayer::UpdateAvailableTextures() {
    availableTextures.clear();
    DIR* dir = opendir("../resources/textures/");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {  // If the entry is a regular file...
                availableTextures.push_back(entry->d_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Failed to open directory ../resources/textures/" << std::endl;
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
