#pragma once
#include "3dStudio.hpp"

/*
 * The MaterialManager is a singleton class that should manage all materials in the scene.
 * Materials are stored in a map, where the key is the mesh name and the value is a map of
 * materials. The key of the inner map is the material id and the value is the material.
 */

class MaterialManager {
   public:
    // Delete the copy constructor and assignment operator
    MaterialManager(const MaterialManager&) = delete;
    MaterialManager& operator=(const MaterialManager&) = delete;

    // Static method to get the instance
    static MaterialManager& getInstance() {
        static MaterialManager instance;
        return instance;
    }

    /**
     * Adds a material to the material map.
     *
     * @param meshName Name of the mesh
     * @param materialId Id of the material
     * @param material The material to add
     */
    void AddMaterial(std::string meshName, GLint materialId, Material material);

    /**
     * Gets a material from the material map.
     *
     * @param meshName Name of the mesh
     * @param id Id of the material
     * @return Material
     */
    Material GetMaterial(std::string meshName, GLint id);

    /**
     * Gets all materials of a mesh from the material map.
     *
     * @param meshName Name of the mesh
     * @return std::unordered_map<GLint, Material>
     */
    std::unordered_map<GLint, Material> GetMeshMaterials(std::string meshName);

    /**
     * Gets the default material.
     *
     * @return The default material
     */
    Material GetDefaultMaterial();

    /**
     * @brief Get the Default Ambient constant
     *
     * @return glm::vec3
     */
    glm::vec3 GetDefaultAmbient();

    /**
     * @brief Set the Default Ambient constant
     *
     * @param ambient
     */
    void SetDefaultAmbient(glm::vec3 ambient);

    /**
     * @brief Get the Default Diffuse constant
     *
     * @return glm::vec3
     */
    glm::vec3 GetDefaultDiffuse();

    /**
     * @brief Set the Default Diffuse constant
     *
     * @param diffuse
     */
    void SetDefaultDiffuse(glm::vec3 diffuse);

    /**
     * @brief Get the Default Specular constant
     *
     * @return glm::vec3
     */
    glm::vec3 GetDefaultSpecular();

    /**
     * @brief Set the Default Specular constant
     *
     * @param specular
     */
    void SetDefaultSpecular(glm::vec3 specular);

    /**
     * @brief Get the Default Shininess constant
     *
     * @return float
     */
    float GetDefaultShininess();

    /**
     * @brief Set the Default Shininess constant
     *
     * @param shininess
     */
    void SetDefaultShininess(float shininess);

   private:
    // Private constructor
    MaterialManager() : materialNames() {}
    std::vector<std::string> materialNames;
    std::unordered_map<std::string, std::unordered_map<GLint, Material>> materialMap;

    // A default material is used when a mesh does not have a material.
    Material defaultMaterial = {
        "default",
        glm::vec3(0.2f, 0.2f, 0.2f),  // ambient
        glm::vec3(0.8f, 0.8f, 0.8f),  // diffuse
        glm::vec3(1.0f, 1.0f, 1.0f),  // specular
        32.0f                         // shininess
    };

    Material worldPlaneMaterial = {
        "worldPlane",
        glm::vec3(0.5f, 0.5f, 0.5f),  // ambient
        glm::vec3(1.0f, 1.0f, 1.0f),  // diffuse
        glm::vec3(0.0f, 0.0f, 0.0f),  // specular
        1.0f                          // shininess
    };
};