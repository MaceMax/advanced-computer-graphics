#pragma once

#include <vr/Group.h>
#include <vr/Scene.h>

namespace vr {

/// Load a given file and add content to the scene
bool load3DModelFile(const std::string& filename, std::shared_ptr<Group>& node, const std::shared_ptr<Shader>& shader);

// Load contents of an xml file into the scene
bool loadSceneFile(const std::string& xmlFile, std::shared_ptr<Scene>& scene);
}  // namespace vr