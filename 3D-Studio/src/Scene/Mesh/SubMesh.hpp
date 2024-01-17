#pragma once
#include "3dStudio.hpp"

/**
 * SubMesh class. This class stores information about a submesh.
 * Mostly used for displaying information about the mesh in the GUI.
 */
class SubMesh {
   public:
    /**
     * @brief Construct a new Sub Mesh object
     *
     * @param name The name of the submesh
     * @param vertexCount The number of vertices of the submesh
     * @param faceCount The number of faces of the submesh
     */
    SubMesh(std::string name, int vertexCount, int faceCount);

    /**
     * @brief Get the name of the submesh
     *
     * @return std::string The name of the submesh
     */
    std::string GetName() const;

    /**
     * @brief Get the number of faces of the submesh
     *
     * @return int The number of faces of the submesh
     */
    int GetFaceCount() const;

    /**
     * @brief Get the number of vertices of the submesh
     *
     * @return int The number of vertices of the submesh
     */
    int GetVertexCount() const;

   private:
    std::string name;
    int numFaces;
    int numVertices;
};