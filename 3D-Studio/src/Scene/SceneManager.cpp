#include "SceneManager.hpp"

#include "3dStudio.hpp"

SceneManager::SceneManager() {
    sceneObjects = std::vector<Mesh>();
    cameras = std::vector<Camera>();
    meshLoader = MeshLoader();

    // Add default camera.
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 2.0f),
                           glm::vec3(0.0, 0.0, 0.0),
                           1920.0f / 1080.0f,
                           0.0001f,
                           100.0f);

    cameras.push_back(camera);

    // Load default mesh.
    Mesh mesh = meshLoader.LoadMesh("../resources/objects/cube_special.obj");
    sceneObjects.push_back(mesh);

    // Add default light directional light.
    lights.push_back(new DirectionalLight(glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(1.0f),
                                          glm::vec3(0.5f, 0.5f, 1.0f)));
}

int SceneManager::AddObject(std::string path) {
    try {
        Mesh mesh = meshLoader.LoadMesh(path);
        sceneObjects.push_back(mesh);
    } catch (const std::invalid_argument &e) {
        return -1;
    }
    return 1;
}

int SceneManager::LoadObject(std::string path) {
    // Ugly, but i don't care.
    try {
        Mesh mesh = meshLoader.LoadMesh(path);
        sceneObjects.clear();
        sceneObjects.push_back(mesh);
    } catch (const std::invalid_argument &e) {
        return -1;
    }

    return 1;
}

void SceneManager::AddLight(Light light) {
    lights.push_back(&light);
}

void SceneManager::ChangeLightType(int index, LightType type) {
    Light *light = lights[index];
    if (light->GetType() == type)
        return;

    if (type == LightType::DIRECTIONAL) {
        lights[index] = new DirectionalLight(light->GetAmbientIntensity(),
                                             light->GetDiffuseIntensity(),
                                             light->GetSpecularIntensity(),
                                             glm::vec3(0.5f, 0.5f, 1.0f));
    } else if (type == LightType::POINT) {
        lights[index] = new PointLight(light->GetAmbientIntensity(),
                                       light->GetDiffuseIntensity(),
                                       light->GetSpecularIntensity(),
                                       glm::vec3(1.0f, 1.0f, 1.0f));
    } else if (type == LightType::SPOT) {
        lights[index] = new SpotLight(light->GetAmbientIntensity(),
                                      light->GetDiffuseIntensity(),
                                      light->GetSpecularIntensity(),
                                      glm::vec3(0.5f, 0.5f, 1.0f),
                                      glm::vec3(1.0f, 1.0f, 1.0f),
                                      12.5f,
                                      15.0f);
    }
}

void SceneManager::AddCamera(Camera camera) {
    cameras.push_back(camera);
}

std::vector<Mesh> &SceneManager::GetSceneObjects() {
    return sceneObjects;
}

std::vector<Light *> SceneManager::GetSceneLights() {
    return lights;
}

WorldPlane &SceneManager::GetWorldPlane() {
    return worldPlane;
}

Skybox &SceneManager::GetSkybox() {
    return skybox;
}

Camera &SceneManager::GetActiveCamera() {
    return cameras[activeCameraIndex];
}

Light *SceneManager::GetSelectedLight() {
    return lights[selectedLightIndex];
}

void SceneManager::SelectLight(int index) {
    selectedLightIndex = index;
}

Mesh &SceneManager::GetSelectedObject() {
    return sceneObjects[selectedObjectIndex];
}

void SceneManager::SelectObject(int index) {
    selectedObjectIndex = index;
}

void SceneManager::ResetScene() {
    for (size_t i = 0; i < sceneObjects.size(); i++) {
        sceneObjects[i].Reset();
    }

    cameras[activeCameraIndex].Reset();
}
