#include "Scene.hpp"

std::shared_ptr<Scene> Scene::instance = nullptr;

std::shared_ptr<Scene> Scene::GetInstance() {
    if (instance == nullptr) {
        instance = std::shared_ptr<Scene>(new Scene());
    }
    return instance;
}

Scene::Scene() {
}