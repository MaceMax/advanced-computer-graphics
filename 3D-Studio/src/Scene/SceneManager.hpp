#pragma once
#include "3dStudio.hpp"
#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "Light.hpp"
#include "Mesh.hpp"
#include "MeshLoader.hpp"
#include "PointLight.hpp"
#include "Skybox.hpp"
#include "SpotLight.hpp"
#include "WorldPlane.hpp"

/**
 * SceneManager class. This class stores all the information about the scene, such as
 * the objects, the lights, the cameras, the ambient light, the selected object, etc.
 * It also provides methods to add objects, lights, cameras, etc.
 */
class SceneManager {
   public:
    SceneManager();

    /**
     * @brief Returns the active camera.
     *
     * @return Camera& The active camera
     */
    Camera& GetActiveCamera();

    /**
     * @brief Returns the selected object.
     *
     * @return Mesh& The selected object
     */
    Mesh& GetSelectedObject();

    /**
     * @brief Returns the selected light.
     *
     * @return Light* The selected light
     */
    Light* GetSelectedLight();

    /**
     * @brief Changes the selected light index.
     *
     * @param index The new selected light index.
     */
    void SelectLight(int index);

    /**
     * @brief Adds an object to the scene.
     *
     * @param path The path to the obj-file.
     * @return int 1, If the object was added successfully. -1, If the object could not be loaded.
     */
    int AddObject(std::string path);

    /**
     * @brief Clears the scene and adds an object to the scene.
     *
     * @param path The path to the obj-file.
     * @return int 1, If the object was loaded successfully. -1, If the object could not be loaded.
     */
    int LoadObject(std::string path);

    /**
     * @brief Adds a light to the scene.
     *
     * @param light The light to add.
     */
    void AddLight(Light light);

    /**
     * @brief Changes the light type of a light.
     *
     * @param index The index of the light.
     * @param type The new light type.
     */
    void ChangeLightType(int index, LightType type);

    /**
     * @brief Adds a camera to the scene.
     *
     * @param camera The camera to add.
     */
    void AddCamera(Camera camera);

    /**
     * @brief Selects an object in the scene.
     *
     * @param index The index of the object to select.
     */
    void SelectObject(int index);

    /**
     * @brief Resets the scene.
     */
    void ResetScene();

    /**
     * @brief Get the Scene Objects
     *
     * @return std::vector<Mesh>& The scene objects
     */
    std::vector<Mesh>& GetSceneObjects();

    /**
     * @brief Get the Scene Lights
     *
     * @return std::vector<Light*> The scene lights
     */
    std::vector<Light*> GetSceneLights();

    /**
     * @brief Get the World Plane object
     *
     * @return WorldPlane& The world plane
     */
    WorldPlane& GetWorldPlane();

    /**
     * @brief Get the Skybox object
     *
     * @return Skybox& The skyboxs
     */
    Skybox& GetSkybox();

   private:
    WorldPlane worldPlane = WorldPlane();
    Skybox skybox = Skybox();
    MeshLoader meshLoader;
    std::vector<Mesh> sceneObjects;
    std::vector<Light*> lights;
    std::vector<Camera> cameras;

    int activeCameraIndex = 0;
    int selectedObjectIndex = 0;
    int selectedLightIndex = 0;
};