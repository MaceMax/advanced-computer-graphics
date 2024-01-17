#define TINYOBJLOADER_IMPLEMENTATION
#include "MeshLoader.hpp"

#include <chrono>

#include "tiny_obj_loader.h"

MeshLoader::MeshLoader() {}

MeshLoader::~MeshLoader() {}

Mesh MeshLoader::LoadMesh(std::string path) {
    std::vector<SubMesh> subMeshes;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "../resources/materials";  // Path to material files
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config)) {
        if (!reader.Error().empty()) {
            throw std::invalid_argument("TinyObjReader: " + reader.Error());
        }
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &tobMaterials = reader.GetMaterials();

    std::set<GLint> materialIds;
    std::unordered_map<Vertex, int, VertexHasher> vertexToIndex;
    std::vector<Vertex> vertices;
    std::vector<glm::vec3> positions, normals, colors;
    std::vector<glm::vec2> textureCoords;
    std::vector<GLuint> indices;
    std::unordered_map<int, std::vector<GLuint>> materialIndices;
    auto start = std::chrono::high_resolution_clock::now();
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)

        int subMeshVertexCount = 0;
        int subMeshFaceCount = 0;
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                positions.push_back(glm::vec3(vx, vy, vz));

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    normals.push_back(glm::vec3(nx, ny, nz));
                } else {
                    normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    textureCoords.push_back(glm::vec2(tx, ty));
                } else {
                    textureCoords.push_back(glm::vec2(-1.0f, -1.0f));
                }

                // Optional: vertex colors
                if (attrib.colors.size() > 0) {
                    tinyobj::real_t red = attrib.colors[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t green = attrib.colors[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t blue = attrib.colors[3 * size_t(idx.vertex_index) + 2];
                    colors.push_back(glm::vec3(red, green, blue));
                } else {
                    // white
                    colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                }

                Vertex vertex = Vertex{positions[positions.size() - 1],
                                       normals[normals.size() - 1],
                                       textureCoords[textureCoords.size() - 1],
                                       colors[colors.size() - 1]};

                auto it = vertexToIndex.find(vertex);
                if (it != vertexToIndex.end()) {
                    // Vertex already exists
                    indices.push_back(it->second);
                    // Add index to material indices
                    materialIndices[shapes[s].mesh.material_ids[f]].push_back(it->second);
                } else {
                    // Vertex does not exist
                    vertices.push_back(vertex);
                    int newIndex = vertices.size() - 1;
                    vertexToIndex[vertex] = newIndex;
                    indices.push_back(newIndex);
                    materialIndices[shapes[s].mesh.material_ids[f]].push_back(newIndex);
                    subMeshVertexCount++;
                }
                materialIds.insert(shapes[s].mesh.material_ids[f]);
                subMeshFaceCount++;
            }
            index_offset += fv;
        }

        std::string meshName = shapes[s].name == "" ? path.substr(path.find_last_of('/') + 1) : shapes[s].name;
        SubMesh subMesh = SubMesh(meshName, subMeshVertexCount, subMeshFaceCount / 3);
        subMeshes.push_back(subMesh);
    }

    MaterialManager &materialManager = MaterialManager::getInstance();
    std::string file = path.substr(path.find_last_of('/') + 1);
    std::string meshName = file.substr(0, file.find_last_of('.'));
    if (tobMaterials.size() > 0) {
        for (size_t i = 0; i < tobMaterials.size(); i++) {
            if (materialIds.find(i) != materialIds.end()) {
                Material material = {tobMaterials[i].name,
                                     glm::vec3(tobMaterials[i].ambient[0], tobMaterials[i].ambient[1], tobMaterials[i].ambient[2]),
                                     glm::vec3(tobMaterials[i].diffuse[0], tobMaterials[i].diffuse[1], tobMaterials[i].diffuse[2]),
                                     glm::vec3(tobMaterials[i].specular[0], tobMaterials[i].specular[1], tobMaterials[i].specular[2]),
                                     tobMaterials[i].shininess,
                                     tobMaterials[i].ambient_texname,
                                     tobMaterials[i].diffuse_texname,
                                     tobMaterials[i].specular_texname,
                                     tobMaterials[i].bump_texname};

                materialManager.AddMaterial(meshName, i, material);
            }
        }
    }

    if (materialIds.size() == 1) {
        materialManager.AddMaterial(meshName, -1, materialManager.GetDefaultMaterial());
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Loading mesh took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    return Mesh(path, subMeshes, vertices, 0, materialIndices);
}