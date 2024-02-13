
#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "vr/Nodes/Node.h"
#include "vr/State/Shader.h"
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
    BoundingBox calculateBoundingBox();

    /**
    Render the whole scene
    */
    void render();

   private:
    /**
     * Private constructor for the scene class.
     */
    Scene();

    std::shared_ptr<RenderVisitor> m_renderVisitor;
    std::shared_ptr<UpdateVisitor> m_updateVisitor;

    static std::shared_ptr<Scene> instance;
    std::shared_ptr<vr::Shader> m_shader;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Group> m_root;
};

}  // namespace vr