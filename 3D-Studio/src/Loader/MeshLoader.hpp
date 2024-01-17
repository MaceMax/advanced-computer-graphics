#include "3dStudio.hpp"
#include "MaterialManager.hpp"
#include "Mesh.hpp"
#include "SubMesh.hpp"

/**
 * MeshLoader class. This class is responsible for loading meshes from files
 * using the tinyobjloader library.
 */
class MeshLoader {
   public:
    MeshLoader();
    ~MeshLoader();

    /**
     * Loads a mesh from a file.
     *
     * @param path Path to the file
     * @return Mesh
     */
    Mesh LoadMesh(std::string path);
};