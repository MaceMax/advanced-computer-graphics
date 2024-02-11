#pragma once

#include <vr/Group.h>
#include <vr/Scene.h>

#include <unordered_map>

namespace vr {

typedef std::unordered_map<std::string, std::shared_ptr<Group>> GeometryMap;
typedef std::unordered_map<std::string, std::shared_ptr<Texture>> TextureMap;

/// Load a given file and add content to the scene
bool load3DModelFile(const std::string& filename,
                     std::shared_ptr<Group>& node,
                     const std::shared_ptr<Shader>& shader,
                     GeometryMap* geometryMap = nullptr);

// Load contents of an xml file into the scene
bool loadSceneFile(const std::string& xmlFile, std::shared_ptr<Scene>& scene);
}  // namespace vr