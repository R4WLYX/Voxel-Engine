#pragma once

#include <vector>

#include <glad/glad.h>

#include "vertex_buffer.hpp"
#include "vertex_layout.hpp"

struct MeshData {
    VertexBuffers buffers;
    std::vector<unsigned int> indices;
    VertexLayout layout;
};

class Mesh {
  public:
    Mesh(const MeshData&);

    GLenum primitive = GL_TRIANGLES;

    size_t index_count = 0;
    GLenum index_type = GL_UNSIGNED_INT;

    size_t attribute_count = 0;
    size_t instance_count = 1;

    const std::vector<GLuint>& vbos() const;

    GLuint ibo() const;
    GLuint vao() const;

    void update_buffer(size_t, const void*, size_t);

    void bind() const;
    void draw() const;

  private:
    std::vector<GLuint> _vertex_buffers;
    GLuint _index_buffer;
    GLuint _vertex_array;
};

inline Mesh::Mesh(const MeshData& data) {
    const std::vector<VertexBufferData>& buffers = data.buffers.buffers;
    const std::vector<unsigned int>& indices = data.indices;
    const VertexLayout& layout = data.layout;

    index_count = indices.size();

    _vertex_buffers.resize(buffers.size(), 0);
    glGenBuffers(_vertex_buffers.size(), _vertex_buffers.data());

    for (size_t i = 0; i < buffers.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffers[i]);
        const void* ptr = buffers[i].data.empty() ? nullptr : buffers[i].data.data();
        glBufferData(GL_ARRAY_BUFFER, buffers[i].data.size(), ptr, buffers[i].usage);
    }

    glGenVertexArrays(1, &_vertex_array);
    glBindVertexArray(_vertex_array);

    glGenBuffers(1, &_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    const std::vector<VertexElement>& elements = layout.elements();
    attribute_count = elements.size();

    for (size_t i = 0; i < elements.size(); i++) {
        const auto& e = elements[i];

        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffers[e.buffer_index]);

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, e.count, e.type, e.normalized, e.stride, reinterpret_cast<void*>(e.offset));

        glVertexAttribDivisor(i, e.divisor);
    }

    glBindVertexArray(0);
}

inline const std::vector<GLuint>& Mesh::vbos() const { return _vertex_buffers; }

inline GLuint Mesh::ibo() const { return _index_buffer; }

inline GLuint Mesh::vao() const { return _vertex_array; }

inline void Mesh::update_buffer(size_t index, const void* data, size_t size) {
    if (index >= _vertex_buffers.size()) return;
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffers[index]);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

inline void Mesh::bind() const {
    glBindVertexArray(_vertex_array);
}

inline void Mesh::draw() const {
    glBindVertexArray(_vertex_array);
    if (instance_count <= 1) glDrawElements(primitive, index_count, index_type, nullptr);
    else glDrawElementsInstanced(primitive, index_count, index_type, nullptr, instance_count);
}