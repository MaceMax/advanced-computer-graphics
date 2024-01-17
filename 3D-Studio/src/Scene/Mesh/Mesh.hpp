#pragma once
#include "3dStudio.hpp"
#include "MaterialManager.hpp"
#include "SubMesh.hpp"
#include "stb_image.h"

/**
 * Mesh class. This class is a container for all the data that is needed to
 * render a mesh.
 */
class Mesh {
   public:
    Mesh();
    /**
     * Construct a new Mesh object. Initializes the VAO, VBO and EBO. It also
     * calculates the normals and texture coordinates if none were provided.
     *
     * @param path Path to the file
     * @param meshes Submeshes of the mesh
     * @param vertices Vertices of the mesh
     * @param texture Texture of the mesh
     * @param materialIndices Material indices of the mesh
     */
    Mesh(std::string path, std::vector<SubMesh> meshes, std::vector<Vertex> vertices, GLuint texture, std::unordered_map<int, std::vector<GLuint>> materialIndices);

    /**
     * Copy constructor.
     *
     * @param other The other mesh
     * @return Mesh& Reference to the new mesh
     */
    Mesh& operator=(const Mesh& other);

    /**
     * Translate the mesh.
     *
     * @param translation The translation vector
     */
    void Translate(glm::vec3 translation);

    /**
     * Rotate the mesh.
     *
     * @param angle The angle
     * @param axis The axis
     */
    void Rotate(float angle, glm::vec3 axis);

    /**
     * Scale the mesh.
     *
     * @param scale The scale vector
     */
    void Scale(glm::vec3 scale);

    /**
     * Get the model matrix.
     *
     * @return glm::mat4 The model matrix
     */
    glm::mat4 GetModelMatrix() const;

    /**
     * @brief Get the Sub Mesh Count.
     *
     * @return int The number of submeshes
     */
    int GetSubMeshCount() const;

    /**
     * @brief Gets the Sub Mesh.
     *
     * @param index The index of the submesh
     * @return SubMesh& The submesh
     */
    SubMesh& GetSubMesh(int index);

    /**
     * @brief Get the Sub Mesh Names.
     *
     * @return std::vector<std::string> The submesh names
     */
    std::vector<std::string> GetSubMeshNames() const;

    /**
     * @brief Get the Name of the mesh.
     *
     * @return std::string The name of the mesh
     */
    std::string GetName() const;

    /**
     * @brief Get the Filename of the mesh.
     *
     * @return std::string The filename of the mesh
     */
    std::string GetFilename() const;

    /**
     * @brief Get the Directory of the mesh.
     *
     * @return std::string The directory of the mesh
     */
    std::string GetDirectory() const;

    /**
     * @brief Get the Texture Name of the mesh.
     *
     * @return std::string The texture name of the mesh
     */
    std::string GetTextureName() const;

    /**
     * @brief Get the Vertex Count
     *
     * @return int The number of vertices
     */
    int GetVertexCount() const;

    /**
     * @brief Get the Face Count
     *
     * @return int The number of faces
     */
    int GetFaceCount() const;

    /**
     * @brief Get the Main Texture id.
     *
     * @return GLuint The texture id
     */
    GLuint& GetMainTexture();

    /**
     * @brief Loads a new texture.
     *
     * @param path Path to the texture
     */
    void LoadTexture(std::string path, GLuint& textureUnit);

    /**
     * @brief Show or hide the texture.
     *
     * @param show Show or hide the texture
     */
    void ShowTexture(bool show);

    /**
     * @brief Check if the texture is showing.
     *
     * @return true The texture is showing
     * @return false The texture is not showing
     */
    bool IsShowingTexture() const;

    /**
     * @brief Reset the mesh to its original state.
     */
    void Reset();

    /**
     * @brief Draw the mesh.
     *
     * @param shaderProgram The shader program
     */
    void Draw(GLuint shaderProgram);

   private:
    MaterialManager& materialManager = MaterialManager::getInstance();
    std::string name;
    std::string textureName;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    std::vector<Vertex> vertices;
    std::vector<SubMesh> meshes;
    std::vector<std::string> subMeshNames;
    std::unordered_map<int, std::vector<GLuint>> materialIndices;
    std::string file;
    std::string directory;
    int vertexCount = 0;
    int faceCount = 0;
    int indexCount = 0;
    bool showTexture = false;

    // Every mesh has its own VAO, VBO and EBO.
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    GLuint mainTexture;
    std::unordered_map<int, GLuint[3]> textures;

    /**
     * Calculates the normals of the mesh.
     */
    void calculateNormals();

    /**
     * Calculates the texture coordinates of the mesh.
     */
    void calculateTexCoords();

    /**
     * @brief Check if the mesh has normals or texture coordinates.
     *
     * @return true
     * @return false
     */
    bool hasNormals() const;

    /**
     * @brief Check if the mesh has texture coordinates.
     *
     * @return true
     * @return false
     */
    bool hasTexCoords() const;

    /**
     * @brief Load the lighting maps if they exist.
     */
    void LoadLightingMaps();

    /**
     * @brief Sets the uniforms of the shader program.
     *
     * @param shaderProgram The shader program
     */
    void SetMaterialUniforms(GLuint shaderProgram, int materialIndex);
};