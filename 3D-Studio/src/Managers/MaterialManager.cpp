#include "MaterialManager.hpp"

void MaterialManager::AddMaterial(std::string meshName, GLint materialId, Material material) {
    // Is this a good way of doing it? Absolutely not. Does it work? Yes. Will I change it? Probably not.
    if (meshName == "worldPlane") {
        materialMap[meshName][materialId] = worldPlaneMaterial;
    } else {
        materialMap[meshName][materialId] = material;
    }
}

Material MaterialManager::GetMaterial(std::string meshName, GLint id) {
    return materialMap[meshName][id];
}

Material MaterialManager::GetDefaultMaterial() {
    return defaultMaterial;
}

std::unordered_map<GLint, Material> MaterialManager::GetMeshMaterials(std::string meshName) {
    return materialMap[meshName];
}

glm::vec3 MaterialManager::GetDefaultAmbient() {
    return defaultMaterial.ambient;
}

void MaterialManager::SetDefaultAmbient(glm::vec3 ambient) {
    for (auto& mesh : materialMap) {
        for (auto& material : mesh.second) {
            if (material.second.name == "default") {
                material.second.ambient = ambient;
            }
        }
    }
}

glm::vec3 MaterialManager::GetDefaultDiffuse() {
    return defaultMaterial.diffuse;
}

void MaterialManager::SetDefaultDiffuse(glm::vec3 diffuse) {
    for (auto& mesh : materialMap) {
        for (auto& material : mesh.second) {
            if (material.second.name == "default") {
                material.second.diffuse = diffuse;
            }
        }
    }
}

glm::vec3 MaterialManager::GetDefaultSpecular() {
    return defaultMaterial.specular;
}

void MaterialManager::SetDefaultSpecular(glm::vec3 specular) {
    for (auto& mesh : materialMap) {
        for (auto& material : mesh.second) {
            if (material.second.name == "default") {
                material.second.specular = specular;
            }
        }
    }
}

float MaterialManager::GetDefaultShininess() {
    return defaultMaterial.shininess;
}

void MaterialManager::SetDefaultShininess(float shininess) {
    // Shitty code, but it works
    for (auto& mesh : materialMap) {
        for (auto& material : mesh.second) {
            if (material.second.name == "default") {
                material.second.shininess = shininess;
            }
        }
    }
}
