
#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "vr/Nodes/Node.h"
#include "vr/State/Shader.h"
#include "vr/Visitors/DepthVisitor.h"
#include "vr/Visitors/RenderVisitor.h"
#include "vr/Visitors/UpdateVisitor.h"

namespace vr {

class Scene;

/**
Class that holds all mesh objects, lights and a camera
*/
class Scene {
   public:
    /**
    Constructor
    */
    Scene(Scene& other) = delete;

    void cleanup();

    /**
     * Assignment operator. Singleton cannot be assigned.
     */
    void operator=(const Scene&) = delete;

    /**
     * Method to get the instance of the singleton.
     */
    static std::shared_ptr<Scene> getInstance();

    /**
    Initialize the vertex and fragment shader and a program.
    \param vshader_filename
    \param fshader_filename
    \return true if successful.
    */
    bool initShaders(const std::string& vshader_filename, const std::string& fshader_filename);

    /**
    Add a lightsource to scene root
    \param light - A new light source<
    */
    void add(std::shared_ptr<Light> light);

    /**
    Get all light sources
    \return A vector of lightsources
    */
    const LightVector getLights();

    /**
     * Set the lightsources in the scene
     */
    void setLights(LightVector lights);

    /**
    Get the camera
    \return The camera
    */
    std::shared_ptr<Camera> getCamera();

    /**
    Use the shader program
    */
    void useProgram();

    std::shared_ptr<Group>& getRoot();

    /**
    Compute a bounding box for the whole scene
    \return A bounding box for the whole scene
    */
    BoundingBox calculateSceneBoundingBox(bool excludeGround = true);

    /**
    Render the whole scene
    */
    void render();

    /**
     * Renders the depth maps for all lights in the scene
     */
    void renderDepthMaps();

    /**
     * Returns the selected light in the scene
     */
    std::shared_ptr<Light> getSelectedLight();

    /**
     * Sets the selected light in the scene
     */
    void setSelectedLight(int index);

    /**
     * Set the ground plane
     */
    void setGroundPlane(std::shared_ptr<Group> ground);

    /**
     * Toggle the shadows in the scene
     */
    void toggleShadows();

   private:
    /**
     * Private constructor for the scene class.
     */
    Scene();

    bool m_shadowsEnabled = true;

    std::shared_ptr<RenderVisitor> m_renderVisitor;
    std::shared_ptr<UpdateVisitor> m_updateVisitor;
    std::shared_ptr<DepthVisitor> m_depthVisitor;
    // LightNode need to be also stored in the scene as they are needed for rendering depth maps
    LightVector m_lights;
    int selectedLight = 0;

    static std::shared_ptr<Scene> instance;
    std::shared_ptr<vr::Shader> m_shader;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Group> m_root;
    std::shared_ptr<Group> m_groundPlane;
};

}  // namespace vr