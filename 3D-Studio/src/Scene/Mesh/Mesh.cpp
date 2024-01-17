#include "Mesh.hpp"

Mesh::Mesh(std::string path, std::vector<SubMesh> meshes, std::vector<Vertex> vertices, GLuint texture, std::unordered_map<int, std::vector<GLuint>> materialIndices) {
    this->vertices = vertices;
    this->meshes = meshes;
    this->materialIndices = materialIndices;
    this->vertexCount = vertices.size();
    this->file = path.substr(path.find_last_of('/') + 1);
    this->name = file.substr(0, file.find_last_of('.'));
    this->directory = path.substr(0, path.find_last_of('/'));

    for (auto& mesh : meshes) {
        subMeshNames.push_back(mesh.GetName());
    }

    // Centralize the mesh in a 2x2x2 cube
    glm::vec3 min = glm::vec3(FLT_MAX);
    glm::vec3 max = glm::vec3(-FLT_MAX);
    for (Vertex& vertex : this->vertices) {
        min.x = glm::min(min.x, vertex.position.x);
        min.y = glm::min(min.y, vertex.position.y);
        min.z = glm::min(min.z, vertex.position.z);

        max.x = glm::max(max.x, vertex.position.x);
        max.y = glm::max(max.y, vertex.position.y);
        max.z = glm::max(max.z, vertex.position.z);
    }

    // Calculate scale and translation. Scale the mesh to fit in a 2x2x2 cube and scale it a bit smaller
    // so it doesn't touch the edges of the cube. Translate the mesh so it's centered in the cube.
    glm::vec3 scale = glm::vec3(2.0f) / (max - min);
    float scaleFactor = glm::min(scale.x, glm::min(scale.y, scale.z));
    scale = glm::vec3(scaleFactor * 0.5f);
    glm::vec3 translation = -(min + (max - min) / 2.0f) * scale;

    // Apply scale and translation to the vertices
    for (Vertex& vertex : this->vertices) {
        vertex.position = vertex.position * scale + translation;
    }

    // Calculate normals
    if (!hasNormals())
        calculateNormals();

    if (!hasTexCoords())
        calculateTexCoords();

    // Load default texture if no texture was loaded
    if (texture == 0) {
        mainTexture = 0;
        LoadTexture("../resources/textures/chrille.png", mainTexture);
    }

    LoadLightingMaps();

    // Generate VAO, VBO and EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    size_t iBufferSize = 0;
    for (auto& pair : materialIndices) {
        iBufferSize += pair.second.size();
    }

    this->indexCount = iBufferSize;
    this->faceCount = this->indexCount / 3;

    // Bind VAO
    glBindVertexArray(vao);
    CheckOpenGLError();

    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);

    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBufferSize * sizeof(GLuint), nullptr, GL_STATIC_DRAW);

    size_t indexOffset = 0;
    for (auto& pair : materialIndices) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexOffset, pair.second.size() * sizeof(GLuint), pair.second.data());
        indexOffset += pair.second.size() * sizeof(GLuint);
    }

    // Set vertex attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Color
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(3);

    // Unbind VAO
    glBindVertexArray(0);
}

Mesh::Mesh() {
}

Mesh& Mesh::operator=(const Mesh& other) {
    if (this != &other) {
        this->vertices = other.vertices;
        this->meshes = other.meshes;
        this->materialIndices = other.materialIndices;
        this->vertexCount = other.vertexCount;
        this->file = other.file;
        this->name = other.name;
        this->directory = other.directory;
        this->indexCount = other.indexCount;
        this->faceCount = other.faceCount;
        this->showTexture = other.showTexture;
        this->vao = other.vao;
        this->vbo = other.vbo;
        this->ebo = other.ebo;
        this->textures = other.textures;
        this->textureName = other.textureName;
        this->subMeshNames = other.subMeshNames;
    }
    return *this;
}

void Mesh::Translate(glm::vec3 translation) {
    if (glm::compMax(translation) != 0 || glm::compMin(translation) != 0) {
        modelMatrix = glm::translate(glm::mat4(1.0f), translation) * modelMatrix;
    }
}

void Mesh::Rotate(float angle, glm::vec3 axis) {
    if (angle != 0.0f) {
        modelMatrix = glm::rotate(modelMatrix, angle, axis);
    }
}

void Mesh::Scale(glm::vec3 scale) {
    if (scale != glm::vec3(1.0f)) {
        modelMatrix = glm::scale(modelMatrix, scale);
    }
}

// Calculate normals for each vertex
void Mesh::calculateNormals() {
    std::vector<GLuint> indices;

    for (auto& pair : materialIndices) {
        std::vector<GLuint> meshIndices = pair.second;
        indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());
    }

    // Calculate normals per triangle and add them to each vertex normal
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v1 = vertices[indices[i]];
        Vertex& v2 = vertices[indices[i + 1]];
        Vertex& v3 = vertices[indices[i + 2]];

        glm::vec3 normal = glm::normalize(glm::cross(v2.position - v1.position, v3.position - v1.position));

        v1.normal += normal;
        v2.normal += normal;
        v3.normal += normal;
    }

    // Normalize each vertex normal (this effectively averages the normals)
    for (Vertex& vertex : vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }
}

// Calculate texture coordinates using two-part UV mapping
void Mesh::calculateTexCoords() {
    // Determine radius of intermediate sphere by finding the vertex
    // with the largest distance from the origin
    float r = 0.0f;
    for (Vertex& vertex : vertices) {
        float length = glm::length(vertex.position);
        if (length > r) {
            r = length;
        }
    }

    // Calculate texture coordinates
    for (Vertex& vertex : vertices) {
        float s = glm::acos(vertex.position.x / r) / glm::pi<float>();
        float t = glm::atan(vertex.position.z / vertex.position.y) / glm::pi<float>() + 0.5f;
        vertex.texCoords = glm::vec2(s, t);
    }
}

GLuint& Mesh::GetMainTexture() {
    return mainTexture;
}

void Mesh::LoadTexture(std::string path, GLuint& textureUnit) {
    // Check if path is absolute or just a filename
    if (path.find('/') == std::string::npos) {
        path = TEXTURE_PATH + path;
    }

    if (textureUnit == mainTexture)
        textureName = path.substr(path.find_last_of('/') + 1);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        stbi_set_flip_vertically_on_load(false);
        glGenTextures(1, &textureUnit);

        glBindTexture(GL_TEXTURE_2D, textureUnit);
        // Set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Load and generate the texture
        // Check if file format supports alpha channel
        if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free the image memory
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
}

void Mesh::LoadLightingMaps() {
    // No error checking here, so make sure the material has a map
    for (auto& pair : materialIndices) {
        int materialId = pair.first;
        Material material = materialManager.GetMaterial(name, materialId);
        if (material.ambientMapPath != "") {
            GLuint texture;
            LoadTexture(material.ambientMapPath, texture);
            textures[materialId][0] = texture;
        }

        if (material.diffuseMapPath != "") {
            GLuint texture;
            LoadTexture(material.diffuseMapPath, texture);
            textures[materialId][1] = texture;
        }

        if (material.specularMapPath != "") {
            GLuint texture;
            LoadTexture(material.specularMapPath, texture);
            textures[materialId][2] = texture;
        }
    }
}

glm::mat4 Mesh::GetModelMatrix() const {
    return modelMatrix;
}

SubMesh& Mesh::GetSubMesh(int index) {
    if (index < 0 || index >= (int)meshes.size()) {
        throw std::invalid_argument("GetSubMesh(): Index out of bounds");
    }
    return meshes[index];
}

int Mesh::GetSubMeshCount() const {
    return meshes.size();
}

std::string Mesh::GetFilename() const {
    return file;
}

std::string Mesh::GetDirectory() const {
    return directory;
}

void Mesh::Reset() {
    modelMatrix = glm::mat4(1.0f);
}

void Mesh::ShowTexture(bool show) {
    showTexture = show;
}

bool Mesh::IsShowingTexture() const {
    return showTexture;
}

std::vector<std::string> Mesh::GetSubMeshNames() const {
    return subMeshNames;
}

std::string Mesh::GetName() const {
    return name;
}

std::string Mesh::GetTextureName() const {
    return textureName;
}

int Mesh::GetVertexCount() const {
    return vertexCount;
}

int Mesh::GetFaceCount() const {
    return faceCount;
}

bool Mesh::hasNormals() const {
    for (const Vertex& vertex : vertices) {
        if (vertex.normal != glm::vec3(0, 0, 0)) {
            return true;
        }
    }
    return false;
}

bool Mesh::hasTexCoords() const {
    for (const Vertex& vertex : vertices) {
        if (vertex.texCoords != glm::vec2(-1, -1)) {
            return true;
        }
    }
    return false;
}

void Mesh::SetMaterialUniforms(GLuint shaderProgram, int materialIndex) {
    Material material = materialManager.GetMaterial(name, materialIndex);

    if (material.ambientMapPath != "") {
        glUniform1i(glGetUniformLocation(shaderProgram, "material.hasAmbientMap"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[materialIndex][0]);

        // Set the sampler2D uniform in the shader to use texture unit 1
        GLint uniformLocation = glGetUniformLocation(shaderProgram, "material.ambientMap");
        glUniform1i(uniformLocation, 1);  // 1 corresponds to GL_TEXTURE1
        CheckOpenGLError();
    } else {
        glUniform1i(glGetUniformLocation(shaderProgram, "material.hasAmbientMap"), 0);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"),
                    material.ambient.x,
                    material.ambient.y,
                    material.ambient.z);
    }

    if (material.diffuseMapPath != "") {
        glUniform1i(glGetUniformLocation(shaderProgram, "material.hasDiffuseMap"), 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textures[materialIndex][1]);
        // Set the sampler2D uniform in the shader to use texture unit 2
        GLint uniformLocation = glGetUniformLocation(shaderProgram, "material.diffuseMap");
        glUniform1i(uniformLocation, 2);  // 2 corresponds to GL_TEXTURE2
        CheckOpenGLError();
    } else {
        glUniform1i(glGetUniformLocation(shaderProgram, "material.hasDiffuseMap"), 0);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"),
                    material.diffuse.x,
                    material.diffuse.y,
                    material.diffuse.z);
    }

    if (material.specularMapPath != "") {
        glUniform1i(glGetUniformLocation(shaderProgram, "material.hasSpecularMap"), 1);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textures[materialIndex][2]);
        // Set the sampler2D uniform in the shader to use texture unit 3
        GLint uniformLocation = glGetUniformLocation(shaderProgram, "material.specularMap");
        glUniform1i(uniformLocation, 3);  // 3 corresponds to GL_TEXTURE3
        CheckOpenGLError();
    } else {
        glUniform1i(glGetUniformLocation(shaderProgram, "material.hasSpecularMap"), 0);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"),
                    material.specular.x,
                    material.specular.y,
                    material.specular.z);
    }

    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"),
                material.shininess);
}

void Mesh::Draw(GLuint shaderProgram) {
    if (showTexture) {
        glUniform1i(glGetUniformLocation(shaderProgram, "hasTexture"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mainTexture);
        // Set the sampler2D uniform in the shader to use texture unit 0
        GLint uniformLocation = glGetUniformLocation(shaderProgram, "texture0");
        glUniform1i(uniformLocation, 0);  // 0 corresponds to GL_TEXTURE0
    } else {
        glUniform1i(glGetUniformLocation(shaderProgram, "hasTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Bind the VAO
    glBindVertexArray(vao);
    CheckOpenGLError();

    // Draw the mesh
    size_t offset = 0;
    for (auto& materialId : materialIndices) {
        // Set the material uniforms
        SetMaterialUniforms(shaderProgram, materialId.first);
        CheckOpenGLError();
        // Draw the mesh
        glDrawElements(GL_TRIANGLES, materialId.second.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
        offset += materialId.second.size();
        CheckOpenGLError();
    }

    CheckOpenGLError();

    // Unbind the VAO
    glBindVertexArray(0);
    CheckOpenGLError();
}
