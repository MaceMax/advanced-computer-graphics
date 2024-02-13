// Important: Import glad as first gl header
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <vr/Callbacks/AnimationCallback.h>
#include <vr/FileSystem.h>
#include <vr/Nodes/Geometry.h>
#include <vr/Nodes/Group.h>
#include <vr/Nodes/LodNode.h>
#include <vr/Nodes/Transform.h>
#include <vr/Scene/Loader.h>
#include <vr/Scene/Scene.h>
#include <vr/State/Material.h>
#include <vr/State/Shader.h>
#include <vr/State/Texture.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <stack>

using namespace vr;

std::string findTexture(const std::string& texturePath, const std::string& modelPath) {
    bool found = vr::FileSystem::exists(texturePath);
    if (found)
        return texturePath;

    std::string newPath;

    std::string vrpath = vr::FileSystem::getEnv("VR_PATH");

    // Search in the same path as the model:
    std::string modelDir = vr::FileSystem::getDirectory(modelPath);
    newPath = modelDir + "/" + texturePath;
    found = vr::FileSystem::exists(newPath);
    if (found)
        return newPath;

    // What if we only keep the last directory name+filename?

    auto directories = vr::FileSystem::splitPath(texturePath);
    auto count = directories.size();
    if (count > 1) {
        newPath = directories[count - 2] + "/" + directories[count - 1];
        found = vr::FileSystem::exists(newPath);
        if (found)
            return newPath;

        // Try add model path also:
        newPath = modelDir + "/" + newPath;
        found = vr::FileSystem::exists(newPath);
        if (found)
            return newPath;

        // Search using the VR_PATH environment variable then.
        newPath = vrpath + "/" + newPath;

        found = vr::FileSystem::exists(newPath);
        if (found)
            return newPath;
    }

    // Search using the VR_PATH environment variable then.
    newPath = vrpath + "/" + texturePath;

    found = vr::FileSystem::exists(newPath);
    if (found)
        return newPath;

    return "";  // Unable to find
}

size_t ExtractMaterials(const aiScene* scene, MaterialVector& materials, const std::string modelPath) {
    uint32_t num_materials = scene->mNumMaterials;
    aiMaterial* ai_material;
    aiColor4D color(0.0f, 0.0f, 0.0f, 1.0f);
    GLfloat shiniess;
    aiString path;

    for (uint32_t i = 0; i < num_materials; i++) {
        std::shared_ptr<Material> material(new Material);

        ai_material = scene->mMaterials[i];
        if (ai_material->GetName().C_Str() == std::string(AI_DEFAULT_MATERIAL_NAME) || ai_material->GetName().C_Str() == std::string("(null)")) {
            materials.push_back(nullptr);
            continue;
        }

        ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material->setAmbient(glm::vec4(color.r, color.g, color.b, color.a));

        ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material->setDiffuse(glm::vec4(color.r, color.g, color.b, color.a));

        ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material->setSpecular(glm::vec4(color.r, color.g, color.b, color.a));

        // ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        // material->setAmbient(glm::vec4(color.r, color.g, color.b, color.a));

        ai_material->Get(AI_MATKEY_SHININESS, shiniess);
        material->setShininess(shiniess);

        if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString res("res\\");
            ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

            std::string texturePath = findTexture(path.C_Str(), modelPath);
            if (texturePath.empty()) {
                std::cerr << "Unable to find texture: " << path.C_Str() << std::endl;
            } else {
                std::shared_ptr<vr::Texture> texture = std::make_shared<vr::Texture>();
                if (!texture->create(texturePath.c_str(), true, 0))
                    std::cerr << "Error creating texture: " << texturePath << std::endl;
                else
                    material->setTexture(texture, 0);
            }
        }

        if (ai_material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            aiString res("res\\");
            path.Clear();
            ai_material->GetTexture(aiTextureType_SPECULAR, 0, &path);
        }

        if (ai_material->GetTextureCount(aiTextureType_HEIGHT) > 0) {
            aiString res("res\\");
            path.Clear();
            ai_material->GetTexture(aiTextureType_HEIGHT, 0, &path);
        }

        if (ai_material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0) {
            aiString res("res\\");
            path.Clear();
            ai_material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path);
        }

        materials.push_back(material);
    }

    return materials.size();
}

glm::mat4 assimpToGlmMatrix(const aiMatrix4x4& ai_matrix) {
    glm::mat4 glm_matrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            glm_matrix[i][j] = ai_matrix[j][i];
        }
    }

    return glm_matrix;
}

void parseNodes(aiNode* root_node, MaterialVector& materials, std::stack<glm::mat4>& transformStack, std::shared_ptr<Group>& node, const aiScene* aiScene, const std::shared_ptr<Shader>& shader) {
    glm::mat4 transform = assimpToGlmMatrix(root_node->mTransformation);

    glm::mat4 m = transformStack.top() * transform;
    transformStack.push(m);

    uint32_t num_meshes = root_node->mNumMeshes;

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec2> tex_coords;

    for (uint32_t i = 0; i < num_meshes; i++) {
        aiMesh* mesh = aiScene->mMeshes[root_node->mMeshes[i]];
        uint32_t num_vertices = mesh->mNumVertices;
        // Create a new mesh

        std::shared_ptr<State> state = std::make_shared<State>();

        std::vector<glm::vec4> vertices(num_vertices);
        std::vector<glm::vec3> normals(num_vertices);
        std::vector<glm::vec2> texCoords(num_vertices);
        std::vector<GLuint> elements;

        // tangents.resize(num_vertices);

        for (uint32_t j = 0; j < num_vertices; j++) {
            vertices[j] = glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1);
            normals[j] = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);

            glm::vec3 tangent;
            if (mesh->HasTangentsAndBitangents()) {
                tangent.x = mesh->mTangents[j].x;
                tangent.y = mesh->mTangents[j].y;
                tangent.z = mesh->mTangents[j].z;
            }

            glm::vec2 tex_coord;
            if (mesh->mTextureCoords[0]) {
                tex_coord.x = mesh->mTextureCoords[0][j].x;
                tex_coord.y = mesh->mTextureCoords[0][j].y;

                texCoords[j] = tex_coord;
            }

            // Ignore color
            //       if (mesh->HasVertexColors(j))
            //       {
            //         vertex.color.x = mesh->mColors[j]->r;
            //         vertex.color.y = mesh->mColors[j]->g;
            //         vertex.color.z = mesh->mColors[j]->b;
            //       }
        }

        uint32_t num_faces = mesh->mNumFaces;
        elements.resize(0);
        for (uint32_t j = 0; j < num_faces; j++) {
            aiFace face = mesh->mFaces[j];
            uint32_t num_indices = face.mNumIndices;
            for (uint32_t k = 0; k < num_indices; k++) {
                elements.push_back(face.mIndices[k]);
            }
        }

        std::shared_ptr<Geometry> loadedMesh(new Geometry(vertices, normals, texCoords, elements, mesh->mName.C_Str()));
        loadedMesh->setInitialTransform(transformStack.top());
        loadedMesh->initShader(shader);
        loadedMesh->upload();

        if (!materials.empty() && materials[mesh->mMaterialIndex] != nullptr)
            state->setMaterial(materials[mesh->mMaterialIndex]);

        loadedMesh->setState(state);

        node->addChild(loadedMesh);
    }

    for (uint32_t i = 0; i < root_node->mNumChildren; i++) {
        parseNodes(root_node->mChildren[i], materials, transformStack, node, aiScene, shader);
    }
    transformStack.pop();
}

bool vr::load3DModelFile(const std::string& filename, std::shared_ptr<Group>& node, const std::shared_ptr<Shader>& shader, GeometryMap* geometryMap) {
    std::string filepath = vr::FileSystem::findFile(filename);
    if (filepath.empty()) {
        std::cerr << "The file " << filename << " does not exist" << std::endl;
        return false;
    }

    std::stack<glm::mat4> transformStack;
    transformStack.push(glm::mat4());

    if (geometryMap->find(filepath) != geometryMap->end()) {
        node->setChildren(geometryMap->at(filepath)->getChildren());
    } else {
        MaterialVector materials;

        Assimp::Importer importer;
        const aiScene* aiScene = importer.ReadFile(filepath,
                                                   aiProcess_CalcTangentSpace |
                                                       aiProcess_GenSmoothNormals |
                                                       aiProcess_Triangulate |
                                                       aiProcess_JoinIdenticalVertices |
                                                       aiProcess_SortByPType);

        aiNode* root_node = aiScene->mRootNode;
        ExtractMaterials(aiScene, materials, filename);
        std::cout << "Found " << materials.size() << " materials" << std::endl;
        parseNodes(root_node, materials, transformStack, node, aiScene, shader);
        if (geometryMap != nullptr)
            geometryMap->insert(std::make_pair(filepath, node));
    }

    transformStack.pop();

    if (node->getChildren().empty())
        std::cerr << " File " << filepath << " did not contain any mesh data" << std::endl;

    return true;
}

template <class T>
T readValue(const std::string& string) {
    std::stringstream ss;
    ss << string;
    T result;
    ss >> result;
    return result;
}

template <>
bool readValue<bool>(const std::string& value) {
    std::string lowerCaseValue = value;
    std::transform(lowerCaseValue.begin(), lowerCaseValue.end(), lowerCaseValue.begin(), ::tolower);
    return lowerCaseValue == "true";
}

std::string pathToString(std::vector<std::string>& path) {
    std::string result;
    for (auto s : path)
        result = result + "/" + s;

    return result;
}

template <class ContainerT>
void tokenize(const std::string& str, ContainerT& tokens,
              const std::string& delimiters = " ", bool trimEmpty = false) {
    std::string::size_type pos, lastPos = 0, length = str.length();

    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while (lastPos < length + 1) {
        pos = str.find_first_of(delimiters, lastPos);
        if (pos == std::string::npos) {
            pos = length;
        }

        if (pos != lastPos || !trimEmpty)
            tokens.push_back(value_type(str.data() + lastPos,
                                        (size_type)pos - lastPos));

        lastPos = pos + 1;
    }
}

template <class T>
bool getVec(T& vec, const std::string& string, const T& def = T()) {
    vec = def;

    std::vector<std::string> tokens;
    tokenize(string, tokens, " ", true);

    if (tokens.size() > 0 && tokens.size() != T::length())
        return false;

    int i = 0;
    for (auto v : tokens) {
        float val = readValue<float>(v);
        vec[i++] = val;
    }

    return true;
}

std::string getAttribute(rapidxml::xml_node<>* node, const std::string& attribute) {
    if (!node)
        return "";

    rapidxml::xml_attribute<>* attrib = node->first_attribute(attribute.c_str());
    if (!attrib)
        return "";

    return attrib->value();
}

LightVector parseLights(std::vector<std::string> xmlpath, rapidxml::xml_node<>* light_node) {
    LightVector lights;
    for (rapidxml::xml_node<>* child = light_node->first_node(); child; child = child->next_sibling()) {
        xmlpath.push_back(child->name());
        std::string name = std::string(child->name());

        if (name != "Light")
            throw std::runtime_error("Node (" + name + ") Lighting node can only contain Light nodes: " + pathToString(xmlpath));

        std::string enabled = getAttribute(child, "enabled");
        bool enabled_val = true;
        if (!enabled.empty())
            enabled_val = readValue<bool>(enabled);

        std::string position = getAttribute(child, "position");
        glm::vec4 pos;
        if (!getVec<glm::vec4>(pos, position))
            throw std::runtime_error("Node (" + name + ") Invalid position in: " + pathToString(xmlpath));

        std::string ambient = getAttribute(child, "ambient");
        glm::vec4 amb;
        if (!getVec<glm::vec4>(amb, ambient))
            throw std::runtime_error("Node (" + name + ") Invalid diffuse in: " + pathToString(xmlpath));

        std::string diffuse = getAttribute(child, "diffuse");
        glm::vec4 diff;
        if (!getVec<glm::vec4>(diff, diffuse))
            throw std::runtime_error("Node (" + name + ") Invalid diffuse in: " + pathToString(xmlpath));

        std::string specular = getAttribute(child, "specular");
        glm::vec4 spec;
        if (!getVec<glm::vec4>(spec, specular))
            throw std::runtime_error("Node (" + name + ") Invalid specular in: " + pathToString(xmlpath));

        std::shared_ptr<Light> light = std::make_shared<Light>(pos, amb, diff, spec);

        std::string constant = getAttribute(child, "constant");
        std::string linear = getAttribute(child, "linear");
        std::string quadratic = getAttribute(child, "quadratic");

        if (!constant.empty() && !linear.empty() && !quadratic.empty()) {
            light->setAttenuation(readValue<float>(constant), readValue<float>(linear), readValue<float>(quadratic));
        }

        light->setEnabled(enabled_val);

        lights.push_back(light);
    }

    return lights;
}

TextureVector parseTextures(std::vector<std::string> xmlpath, rapidxml::xml_node<>* texture_node) {
    TextureVector textures;
    int slot = 0;
    for (rapidxml::xml_node<>* child = texture_node->first_node(); child; child = child->next_sibling()) {
        xmlpath.push_back(child->name());
        std::string name = std::string(child->name());

        if (name != "Texture")
            throw std::runtime_error("Node (" + name + ") Textures node can only contain Texture nodes: " + pathToString(xmlpath));

        std::string filepath = getAttribute(child, "filepath");
        if (filepath.empty())
            throw std::runtime_error("Node (" + name + ") No filepath specified for Texture: " + pathToString(xmlpath));

        std::shared_ptr<Texture> texture = std::make_shared<Texture>();
        if (!texture->create(filepath.c_str(), false, slot))
            throw std::runtime_error("Node (" + name + ") Error creating texture: " + filepath);

        textures.push_back(texture);
        slot++;
    }

    if (textures.size() > MAX_TEXTURES)
        throw std::runtime_error("Node (Textures) Too many textures, maximum allowed is 10: " + pathToString(xmlpath));

    return textures;
}

State parseState(std::vector<std::string> xmlpath, rapidxml::xml_node<>* state_node) {
    State state = State();
    xmlpath.push_back(state_node->name());

    rapidxml::xml_node<>* lightingNode = state_node->first_node("Lighting");
    if (lightingNode) {
        xmlpath.push_back(lightingNode->name());
        std::string enabled;
        if (lightingNode->first_attribute("enabled"))
            enabled = lightingNode->first_attribute("enabled")->value();

        if (!enabled.empty())
            state.setLightingEnabled(readValue<bool>(enabled));

        LightVector lights = parseLights(xmlpath, lightingNode);
        for (auto l : lights)
            state.addLight(l);
    }

    rapidxml::xml_node<>* shaderNode = state_node->first_node("Shader");
    if (shaderNode) {
        xmlpath.push_back(shaderNode->name());
        std::string vshader;
        if (shaderNode->first_attribute("vshader"))
            vshader = shaderNode->first_attribute("vshader")->value();

        if (vshader.empty())
            throw std::runtime_error("Node (Shader) No filepath specified for vertex shader: " + pathToString(xmlpath));

        std::string fshader;
        if (shaderNode->first_attribute("fshader"))
            fshader = shaderNode->first_attribute("fshader")->value();

        if (fshader.empty())
            throw std::runtime_error("Node (Shader) No filepath specified for fragment shader: " + pathToString(xmlpath));

        // Should probably add shader to a map and check if it already exists to avoid loading the same shader multiple times
        Shader shader = Shader(vshader, fshader);
        if (!shader.valid())
            throw std::runtime_error("Node (Shader) Invalid shader: " + pathToString(xmlpath));

        state.setShader(std::make_shared<Shader>(vshader, fshader));
    }

    rapidxml::xml_node<>* textureNode = state_node->first_node("Textures");
    if (textureNode) {
        xmlpath.push_back(textureNode->name());
        std::string vshader;
        TextureVector textures = parseTextures(xmlpath, textureNode);

        for (auto t : textures)
            state.addTexture(t);
    }

    rapidxml::xml_node<>* materialNode = state_node->first_node("Material");
    if (materialNode) {
        xmlpath.push_back(materialNode->name());

        glm::vec4 amb;
        std::string ambient;
        if (materialNode->first_attribute("ambient"))
            ambient = materialNode->first_attribute("ambient")->value();

        if (ambient.empty())
            throw std::runtime_error("Node (Material) No ambient specified for Material: " + pathToString(xmlpath));

        if (!getVec<glm::vec4>(amb, ambient))
            throw std::runtime_error("Node (Material) Invalid ambient in: " + pathToString(xmlpath));

        glm::vec4 diff;
        std::string diffuse;
        if (materialNode->first_attribute("diffuse"))
            diffuse = materialNode->first_attribute("diffuse")->value();

        if (diffuse.empty())
            throw std::runtime_error("Node (Material) No diffuse specified for Material: " + pathToString(xmlpath));

        if (!getVec<glm::vec4>(diff, diffuse))
            throw std::runtime_error("Node (Material) Invalid diffuse in: " + pathToString(xmlpath));

        glm::vec4 spec;
        std::string specular;
        if (materialNode->first_attribute("specular"))
            specular = materialNode->first_attribute("specular")->value();

        if (specular.empty())
            throw std::runtime_error("Node (Material) No specular specified for Material: " + pathToString(xmlpath));

        if (!getVec<glm::vec4>(spec, specular))
            throw std::runtime_error("Node (Material) Invalid specular in: " + pathToString(xmlpath));

        std::string shininess;
        if (materialNode->first_attribute("shininess"))
            shininess = materialNode->first_attribute("shininess")->value();

        if (shininess.empty())
            throw std::runtime_error("Node (Material) No shininess specified for Material: " + pathToString(xmlpath));

        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->setAmbient(amb);
        material->setDiffuse(diff);
        material->setSpecular(spec);
        material->setShininess(readValue<float>(shininess));
        state.setMaterial(material);
    }

    rapidxml::xml_node<>* cullNode = state_node->first_node("CullFace");
    if (cullNode) {
        xmlpath.push_back(cullNode->name());
        std::string enabled;
        if (cullNode->first_attribute("enabled"))
            enabled = cullNode->first_attribute("enabled")->value();

        if (!enabled.empty())
            state.setCullFaceEnabled(readValue<bool>(enabled));
    }

    return state;
}

LodNode parseLodNode(std::vector<std::string> xmlpath, rapidxml::xml_node<>* lod_node, GeometryMap& geometryMap, const std::shared_ptr<Shader>& shader) {
    std::string nodeName;
    if (lod_node->first_attribute("name"))
        nodeName = lod_node->first_attribute("name")->value();

    LodNode lodNode = nodeName.empty() ? LodNode() : LodNode(nodeName);
    std::string maxDist;

    if (lod_node->first_attribute("maxDistance"))
        maxDist = lod_node->first_attribute("maxDistance")->value();

    if (!maxDist.empty())
        lodNode.setMaxDistance(readValue<float>(maxDist));

    for (rapidxml::xml_node<>* child = lod_node->first_node(); child; child = child->next_sibling()) {
        xmlpath.push_back(child->name());
        std::string name = std::string(child->name());
        std::string nodeName = child->first_attribute("name")->value();

        if (name != "Geometry")
            throw std::runtime_error("Node (" + name + ") LOD node can only contain Geometry nodes: " + pathToString(xmlpath));

        std::string distanceStr;
        if (child->first_attribute("distance"))
            distanceStr = child->first_attribute("distance")->value();

        if (distanceStr.empty() && maxDist.empty())
            throw std::runtime_error("Node (" + name + ") No distance specified for Geometry or max distance specified: " + pathToString(xmlpath));

        float distance = 0;
        if (!distanceStr.empty())
            distance = readValue<float>(distanceStr);

        std::string filepath = child->first_attribute("filepath")->value();

        if (filepath.empty())
            throw std::runtime_error("Node (" + name + ") No filepath specified for Geometry: " + pathToString(xmlpath));

        // Check if we have a geometry with the filepath already in the map
        std::shared_ptr<Group> geometryGroup = std::make_shared<Group>(filepath);
        if (geometryMap.find(filepath) != geometryMap.end()) {
            lodNode.addChild(distance, geometryMap[filepath]);
        } else if (load3DModelFile(filepath, geometryGroup, shader, &geometryMap)) {
            lodNode.addChild(distance, geometryGroup);
        } else {
            throw std::runtime_error("Node (" + name + ") Invalid file in: " + pathToString(xmlpath));
        }
    }

    return lodNode;
}

UpdateCallbackVector parseUpdateCallback(std::vector<std::string> xmlpath, rapidxml::xml_node<>* update_node) {
    std::string name;
    if (update_node->first_attribute("name"))
        name = update_node->first_attribute("name")->value();
    UpdateCallbackVector updateCallbacks;

    for (rapidxml::xml_node<>* child = update_node->first_node(); child; child = child->next_sibling()) {
        xmlpath.push_back(child->name());

        std::string name = std::string(child->name());
        std::string speed = getAttribute(child, "speed");
        float speed_val = 1.0f;
        if (!speed.empty())
            speed_val = readValue<float>(speed);

        std::string loop = getAttribute(child, "loop");
        bool loop_val = true;
        if (!loop.empty())
            loop_val = readValue<bool>(loop);

        std::shared_ptr<AnimationCallback> animCallback = std::make_shared<AnimationCallback>(speed_val, loop_val);

        if (name == "AnimationCallback") {
            for (rapidxml::xml_node<>* frame = child->first_node(); frame; frame = frame->next_sibling()) {
                xmlpath.push_back(frame->name());
                std::string name = std::string(frame->name());
                if (name != "Frame")
                    throw std::runtime_error("Node (" + name + ") Update node can only contain Frame nodes: " + pathToString(xmlpath));

                std::string duration = getAttribute(frame, "duration");
                float duration_val = 0;
                if (!duration.empty())
                    duration_val = readValue<float>(duration);

                std::string translate = getAttribute(frame, "translate");
                glm::vec3 t_vec;
                if (!getVec<glm::vec3>(t_vec, translate))
                    throw std::runtime_error("Node (" + name + ") Invalid translate in: " + pathToString(xmlpath));

                std::string rotate = getAttribute(frame, "rotate");
                glm::vec3 r_vec;
                if (!getVec<glm::vec3>(r_vec, rotate))
                    throw std::runtime_error("Node (" + name + ") Invalid rotate in: " + pathToString(xmlpath));

                std::string scale = getAttribute(frame, "scale");
                glm::vec3 s_vec;
                if (!getVec<glm::vec3>(s_vec, scale, glm::vec3(1)))
                    throw std::runtime_error("Node (" + name + ") Invalid scale in: " + pathToString(xmlpath));

                glm::mat4 mt = glm::translate(glm::mat4(), t_vec);
                glm::mat4 ms = glm::scale(glm::mat4(), s_vec);
                glm::mat4 rx = glm::rotate(glm::mat4(), glm::radians(r_vec.x), glm::vec3(1, 0, 0));
                glm::mat4 ry = glm::rotate(glm::mat4(), glm::radians(r_vec.y), glm::vec3(0, 1, 0));
                glm::mat4 rz = glm::rotate(glm::mat4(), glm::radians(r_vec.z), glm::vec3(0, 0, 1));

                auto t = mt * rz * ry * rx;
                t = glm::scale(t, s_vec);

                AnimationFrame animFrame = {t, duration_val};

                animCallback->addFrame(animFrame);
            }
        } else {
            throw std::runtime_error("Node (" + name + ") Invalid update callback in: " + pathToString(xmlpath));
        }

        updateCallbacks.push_back(animCallback);
    }

    return updateCallbacks;
}
// Parses XML nodes recursively
void parseSceneNode(std::vector<std::string> xmlpath, rapidxml::xml_node<>* node_node, GeometryMap& geometryMap, std::shared_ptr<Group>& node, const std::shared_ptr<Shader>& shader) {
    if (node_node->type() == rapidxml::node_comment || node_node->type() == rapidxml::node_doctype)
        return;

    for (rapidxml::xml_node<>* child = node_node->first_node(); child; child = child->next_sibling()) {
        xmlpath.push_back(child->name());
        std::string name = std::string(child->name());
        if (child->type() == rapidxml::node_comment || child->type() == rapidxml::node_doctype)
            continue;
        std::string nodeName;
        if (child->first_attribute("name"))
            nodeName = child->first_attribute("name")->value();

        rapidxml::xml_node<>* stateNode = child->first_node("State");
        std::shared_ptr<State> state;
        std::shared_ptr<Shader> newShader = shader;
        // Check if we have a state
        if (stateNode) {
            state = std::make_shared<State>(parseState(xmlpath, stateNode));
            newShader = state->getShader() ? state->getShader() : shader;
        }

        if (name == "Group") {
            std::shared_ptr<Group> groupNode = nodeName.empty() ? std::make_shared<Group>() : std::make_shared<Group>(nodeName);
            parseSceneNode(xmlpath, child, geometryMap, groupNode, newShader);
            if (state)
                groupNode->setState(state);
            node->addChild(groupNode);

            rapidxml::xml_node<>* callbacksNode = child->first_node("Callbacks");
            if (callbacksNode) {
                xmlpath.push_back(callbacksNode->name());
                UpdateCallbackVector updateCallbacks = parseUpdateCallback(xmlpath, callbacksNode);
                for (auto c : updateCallbacks)
                    groupNode->addUpdateCallback(c);
            }

        } else if (name == "Transform") {
            std::shared_ptr<Transform> transformNode = std::make_shared<Transform>(nodeName);
            xmlpath.push_back("transform");

            std::string translate = getAttribute(child, "translate");
            glm::vec3 t_vec;
            if (!getVec<glm::vec3>(t_vec, translate))
                throw std::runtime_error("Node (" + name + ") Invalid translate in: " + pathToString(xmlpath));

            std::string rotate = getAttribute(child, "rotate");
            glm::vec3 r_vec;
            if (!getVec<glm::vec3>(r_vec, rotate))
                throw std::runtime_error("Node (" + name + ") Invalid rotate in: " + pathToString(xmlpath));

            std::string scale = getAttribute(child, "scale");
            glm::vec3 s_vec;
            if (!getVec<glm::vec3>(s_vec, scale, glm::vec3(1)))
                throw std::runtime_error("Node (" + name + ") Invalid scale in: " + pathToString(xmlpath));

            glm::mat4 mt = glm::translate(glm::mat4(), t_vec);
            glm::mat4 ms = glm::scale(glm::mat4(), s_vec);
            glm::mat4 rx = glm::rotate(glm::mat4(), glm::radians(r_vec.x), glm::vec3(1, 0, 0));
            glm::mat4 ry = glm::rotate(glm::mat4(), glm::radians(r_vec.y), glm::vec3(0, 1, 0));
            glm::mat4 rz = glm::rotate(glm::mat4(), glm::radians(r_vec.z), glm::vec3(0, 0, 1));

            auto t = mt * rz * ry * rx;
            t = glm::scale(t, s_vec);
            transformNode->setMatrix(t);

            rapidxml::xml_node<>* callbacksNode = child->first_node("Callbacks");
            if (callbacksNode) {
                xmlpath.push_back(callbacksNode->name());
                UpdateCallbackVector updateCallbacks = parseUpdateCallback(xmlpath, callbacksNode);
                for (auto c : updateCallbacks)
                    transformNode->addUpdateCallback(c);
            }

            std::shared_ptr<Group> groupTransform = std::dynamic_pointer_cast<Group>(transformNode);

            parseSceneNode(xmlpath, child, geometryMap, groupTransform, newShader);

            if (state)
                transformNode->setState(state);
            node->addChild(transformNode);

        } else if (name == "Geometry") {
            std::string filepath = child->first_attribute("filepath")->value();
            if (filepath.empty())
                throw std::runtime_error("Node (" + name + ") No filepath specified for Geometry: " + pathToString(xmlpath));

            // Check if we have a geometry with the filepath already in the map
            std::shared_ptr<Group> geometryGroup = std::make_shared<Group>(filepath);
            if (state)
                geometryGroup->setState(state);

            if (load3DModelFile(filepath, geometryGroup, newShader, &geometryMap)) {
                node->addChild(geometryGroup);
            } else {
                throw std::runtime_error("Node (" + name + ") Invalid file in: " + pathToString(xmlpath));
            }

            rapidxml::xml_node<>* callbacksNode = child->first_node("Callbacks");
            if (callbacksNode) {
                xmlpath.push_back(callbacksNode->name());
                UpdateCallbackVector updateCallbacks = parseUpdateCallback(xmlpath, callbacksNode);
                for (auto c : updateCallbacks)
                    geometryGroup->addUpdateCallback(c);
            }

        } else if (name == "LOD") {
            LodNode lodNode = parseLodNode(xmlpath, child, geometryMap, newShader);
            std::shared_ptr<LodNode> lod = std::make_shared<LodNode>(lodNode);

            if (state)
                lod->setState(state);
            node->addChild(lod);

            rapidxml::xml_node<>* callbacksNode = child->first_node("Callbacks");
            if (callbacksNode) {
                xmlpath.push_back(callbacksNode->name());
                UpdateCallbackVector updateCallbacks = parseUpdateCallback(xmlpath, callbacksNode);
                for (auto c : updateCallbacks)
                    lod->addUpdateCallback(c);
            }
        }

        xmlpath.pop_back();
    }
}
bool vr::loadSceneFile(const std::string& sceneFile, std::shared_ptr<Scene>& scene) {
    std::string filepath = sceneFile;
    bool exist = vr::FileSystem::exists(filepath);

    std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
    if (vrPath.empty())
        std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;

    if (!exist && !vrPath.empty()) {
        filepath = std::string(vrPath) + "/" + sceneFile;
        exist = vr::FileSystem::exists(filepath);
    }

    if (!exist) {
        std::cerr << "The file " << sceneFile << " does not exist" << std::endl;
        return false;
    }

    rapidxml::xml_node<>* root_node = nullptr;
    std::vector<std::string> xmlpath;

    try {
        rapidxml::file<> xmlFile(filepath.c_str());  // Default template is char
        rapidxml::xml_document<> doc;

        doc.parse<rapidxml::parse_trim_whitespace | rapidxml::parse_normalize_whitespace | rapidxml::parse_full>(xmlFile.data());

        root_node = doc.first_node("Scene");

        if (!root_node)
            throw std::runtime_error("File missing scene/");

        xmlpath.push_back("Scene");

        rapidxml::xml_node<>* stateNode = root_node->first_node("State");
        if (stateNode) {
            std::shared_ptr<State> rootState = scene->getRoot()->getState();
            std::shared_ptr<State> newRootState = std::make_shared<State>(parseState(xmlpath, stateNode));
            scene->getRoot()->setState(*(rootState) + *(newRootState));
        }

        GeometryMap geometryMap;
        parseSceneNode(xmlpath, root_node, geometryMap, scene->getRoot(), scene->getRoot()->getState()->getShader());

        xmlpath.pop_back();  // scene
    } catch (rapidxml::parse_error& error) {
        std::cerr << "XML parse error: " << error.what() << std::endl;
        return false;
    } catch (std::runtime_error& error) {
        std::cerr << "XML parse error: " << error.what() << std::endl;
        return false;
    }

    return true;
}
