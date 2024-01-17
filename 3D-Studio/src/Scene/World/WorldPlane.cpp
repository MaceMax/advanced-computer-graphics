#include "WorldPlane.hpp"

WorldPlane::WorldPlane() {
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;

    int gridSize = 100;
    float step = 1.0f / static_cast<float>(gridSize);

    for (int i = -gridSize / 2; i <= gridSize / 2; ++i) {
        for (int j = -gridSize / 2; j <= gridSize / 2; ++j) {
            float x = static_cast<float>(i) * step;
            float y = 0.0f;
            float z = static_cast<float>(j) * step;

            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    for (GLuint i = 0; i < static_cast<GLuint>(vertices.size()) - gridSize - 1; ++i) {
        if ((i + 1) % gridSize == 0) {
            continue;  // Skip the last column
        }

        indices.push_back(i);
        indices.push_back(i + gridSize + 1);
        indices.push_back(i + 1);

        indices.push_back(i + 1);
        indices.push_back(i + gridSize + 1);
        indices.push_back(i + gridSize + 2);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    length = indices.size();
}

void WorldPlane::Draw() {
    glBindVertexArray(vao);
    CheckOpenGLError();
    glDrawElements(GL_LINES, length, GL_UNSIGNED_INT, nullptr);
    CheckOpenGLError();
    glBindVertexArray(0);
}