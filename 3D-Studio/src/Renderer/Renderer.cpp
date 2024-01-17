#include "Renderer.hpp"

Renderer::Renderer(SceneManager* sceneManager) {
    this->sceneManager = sceneManager;
    // Create shader programs
    shaderPrograms = std::vector<GLuint>(7);
    shaderPrograms[PHONG] = CreateShader("../shader/Phong/vshader.glsl", "../shader/Phong/fshader.glsl");
    shaderPrograms[GOURAUD] = CreateShader("../shader/Gouraud/vshader.glsl", "../shader/Gouraud/fshader.glsl");
    shaderPrograms[WIREFRAME] = CreateShader("../shader/Wireframe/vshader.glsl", "../shader/Wireframe/fshader.glsl");
    shaderPrograms[TOON] = CreateShader("../shader/Toon/vshader.glsl", "../shader/Toon/fshader.glsl");
    shaderPrograms[SKYBOX] = CreateShader("../shader/Skybox/vshader.glsl", "../shader/Skybox/fshader.glsl");
    shaderPrograms[WORLD_PLANE] = CreateShader("../shader/WorldPlane/vshader.glsl", "../shader/WorldPlane/fshader.glsl");
    shaderProgram = shaderPrograms[PHONG];

    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);
    // Enable blending
    glEnable(GL_BLEND);
    // Set the blend function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //  Create framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create renderbuffer
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    // Create texture
    glGenTextures(1, &viewportTexture);
    glBindTexture(GL_TEXTURE_2D, viewportTexture);
    // Width and height should be the same as the viewport
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // GL_LINEAR
    glGenerateMipmap(GL_TEXTURE_2D);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, viewportTexture, 0);

    // Attach renderbuffer to framebuffer.  // Width and height should be the same as the viewport
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Set shader uniforms
    locM = glGetUniformLocation(shaderProgram, "M");
    locV = glGetUniformLocation(shaderProgram, "V");
    locP = glGetUniformLocation(shaderProgram, "P");
    locVertices = glGetAttribLocation(shaderProgram, "vPosition");
    locNormals = glGetAttribLocation(shaderProgram, "vNormal");
    locTexCoords = glGetAttribLocation(shaderProgram, "vTexCoords");

    glUseProgram(0);
}

Renderer::~Renderer() {
    std::cout << "Destroying Renderer" << std::endl;
    glDeleteProgram(shaderProgram);
}

void Renderer::ChangeShaderProgram(ShaderProgram shadingModel) {
    if (shadingModel == WIREFRAME) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);
    }
    shaderProgram = shaderPrograms[shadingModel];
    glUseProgram(shaderProgram);
    locM = glGetUniformLocation(shaderProgram, "M");
    locV = glGetUniformLocation(shaderProgram, "V");
    locP = glGetUniformLocation(shaderProgram, "P");
    locVertices = glGetAttribLocation(shaderProgram, "vPosition");
    locNormals = glGetAttribLocation(shaderProgram, "vNormal");
    locTexCoords = glGetAttribLocation(shaderProgram, "vTexCoords");
    glUseProgram(0);
}

void Renderer::ShowBackface(bool showBackface) {
    if (showBackface) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
    }
}

void Renderer::ShowSkybox(bool showSkybox) {
    renderSkybox = showSkybox;
}

void Renderer::SetToonLevels(int toonLevels) {
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "toonLevels"), toonLevels);
    glUseProgram(0);
}

void Renderer::RescaleFramebuffer(float width, float height) {
    glBindTexture(GL_TEXTURE_2D, viewportTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

GLuint Renderer::ViewportTexture() const {
    return viewportTexture;
}

std::string Renderer::ReadShaderSource(const std::string shaderFile) {
    std::string shaderSource;
    std::string line;

    std::fstream fs(shaderFile, std::ios::in);
    if (!fs)
        return shaderSource;

    while (!fs.eof()) {
        getline(fs, line);
        shaderSource.append(line + '\n');
    }
    fs.close();
    return shaderSource;
}

GLuint Renderer::CreateShader(const std::string vShaderFile, const std::string fShaderFile) {
    GLuint program;
    int i;
    GLint linked;

    struct shaders_t {
        std::string filename;
        GLenum type;
    };

    shaders_t shaders[2] = {
        {vShaderFile, GL_VERTEX_SHADER},
        {fShaderFile, GL_FRAGMENT_SHADER}};

    program = glCreateProgram();
    for (i = 0; i < 2; ++i) {
        GLuint shader;
        GLint compiled;

        std::string shaderSource = ReadShaderSource(shaders[i].filename);
        if (shaderSource.empty()) {
            std::cerr << "Failed to read " << shaders[i].filename << std::endl;
            exit(EXIT_FAILURE);
        }

        shader = glCreateShader(shaders[i].type);
        const char* shaderSrc = shaderSource.c_str();
        glShaderSource(shader, 1, &shaderSrc, NULL);
        glCompileShader(shader);
        CheckOpenGLError();

        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint logSize;

            std::cerr << "Failed to compile " << shaders[i].filename << std::endl;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
            if (logSize > 0) {
                char logMsg[logSize + 1];
                glGetShaderInfoLog(shader, logSize, nullptr, &(logMsg[0]));
                std::cerr << "Shader info log: " << logMsg << std::endl;
            }
            exit(EXIT_FAILURE);
        }
        glAttachShader(program, shader);
    }

    /* link  and error check */
    glLinkProgram(program);
    CheckOpenGLError();

    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint logSize;

        std::cerr << "Shader program failed to link!" << std::endl;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            char logMsg[logSize + 1];
            glGetProgramInfoLog(program, logSize, NULL, &(logMsg[0]));
            std::cerr << "Program info log: " << logMsg << std::endl;
        }
        exit(EXIT_FAILURE);
    }
    return program;
}

void Renderer::RenderScene() {
    // Get the list of objects from the scene manager
    std::vector<Mesh>& objects = sceneManager->GetSceneObjects();
    glm::mat4 viewMatrix = sceneManager->GetActiveCamera().GetViewMatrix();
    glm::mat4 projectionMatrix = sceneManager->GetActiveCamera().GetProjectionMatrix();

    glUseProgram(shaderProgram);

    for (Mesh& mesh : objects) {
        glUniformMatrix4fv(locM, 1, GL_FALSE, glm::value_ptr(mesh.GetModelMatrix()));
        glUniformMatrix4fv(locV, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(locP, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        CheckOpenGLError();
        mesh.Draw(shaderProgram);
        CheckOpenGLError();
    }

    RenderLights();

    // Set camera position
    glm::vec3 cameraPos = sceneManager->GetActiveCamera().GetPosition();
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUseProgram(0);
}

void Renderer::RenderLights() {
    Light* light = sceneManager->GetSelectedLight();

    glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"),
                light->GetAmbientIntensity().x,
                light->GetAmbientIntensity().y,
                light->GetAmbientIntensity().z);

    glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"),
                light->GetDiffuseIntensity().x,
                light->GetDiffuseIntensity().y,
                light->GetDiffuseIntensity().z);

    glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"),
                light->GetSpecularIntensity().x,
                light->GetSpecularIntensity().y,
                light->GetSpecularIntensity().z);

    LightType type = light->GetType();
    glUniform1i(glGetUniformLocation(shaderProgram, "light.type"), static_cast<GLint>(type));
    if (type == LightType::DIRECTIONAL) {
        DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(light);

        glUniform4f(glGetUniformLocation(shaderProgram, "light.position"),
                    directionalLight->GetDirection().x,
                    directionalLight->GetDirection().y,
                    directionalLight->GetDirection().z,
                    0.0f);  // Directional light has no position so w is 0.0f

        glUniform3f(glGetUniformLocation(shaderProgram, "lightDir"), directionalLight->GetDirection().x, directionalLight->GetDirection().y, directionalLight->GetDirection().z);
    } else if (type == LightType::POINT) {
        PointLight* pointLight = dynamic_cast<PointLight*>(light);

        glUniform4f(glGetUniformLocation(shaderProgram, "light.position"),
                    pointLight->GetPosition().x,
                    pointLight->GetPosition().y,
                    pointLight->GetPosition().z,
                    1.0f);  // Point light has a position so w is 1.0f

        glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), pointLight->GetConstant());
        glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), pointLight->GetLinear());
        glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), pointLight->GetQuadratic());
    }
    CheckOpenGLError();
}

void Renderer::RenderSkybox() {
    if (!renderSkybox)
        return;

    if (shaderProgram == shaderPrograms[WIREFRAME])
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Remove translation from view matrix
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(sceneManager->GetActiveCamera().GetViewMatrix()));
    glm::mat4 projectionMatrix = sceneManager->GetActiveCamera().GetProjectionMatrix();

    glDepthFunc(GL_LEQUAL);
    glUseProgram(shaderPrograms[SKYBOX]);
    glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[SKYBOX], "V"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[SKYBOX], "P"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    sceneManager->GetSkybox().Draw();

    glDepthFunc(GL_LESS);
    if (shaderProgram == shaderPrograms[WIREFRAME])
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glUseProgram(0);
}

void Renderer::RenderWorldPlane() {
    if (!renderWorldPlane)
        return;

    if (shaderProgram == shaderPrograms[WIREFRAME])
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::mat4 viewMatrix = glm::mat4(glm::mat3(sceneManager->GetActiveCamera().GetViewMatrix()));
    glm::mat4 projectionMatrix = sceneManager->GetActiveCamera().GetProjectionMatrix();

    glUseProgram(shaderPrograms[WORLD_PLANE]);
    glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[WORLD_PLANE], "V"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[WORLD_PLANE], "P"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    sceneManager->GetWorldPlane().Draw();

    if (shaderProgram == shaderPrograms[WIREFRAME])
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram(0);
}

void Renderer::Render() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //  Use the shader program

    // Clear the color and depth buffers
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderSkybox();
    // Render the scene objects
    RenderScene();
    // Render the world plane
    // RenderWorldPlane();
    // Render the skybox

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}