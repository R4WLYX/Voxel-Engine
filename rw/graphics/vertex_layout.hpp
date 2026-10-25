#pragma once

#include <vector>

#include <glad/glad.h>

struct VertexElement {
    GLenum type;
    size_t count;
    bool normalized;

    size_t stride;
    size_t offset;
    size_t buffer_index;
    size_t divisor;
};

struct VertexLayout {
  public:
    template <typename T>
    void push(size_t, size_t = 0, size_t = 0);

    void finalize();

    const std::vector<VertexElement>& elements() const;

  private:
    std::vector<VertexElement> _elements;
    std::vector<size_t> _strides;

    void ensure_strides(size_t);

    size_t get_sizeof(GLenum);
};

template <>
inline void VertexLayout::push<float>(size_t count, size_t buffer_index, size_t divisor) {
    ensure_strides(buffer_index);
    size_t offset = _strides[buffer_index];

    _elements.push_back({GL_FLOAT, count, GL_FALSE, 0, offset, buffer_index, divisor});

    _strides[buffer_index] += get_sizeof(GL_FLOAT) * count;
}

template <>
inline void VertexLayout::push<unsigned int>(size_t count, size_t buffer_index, size_t divisor) {
    ensure_strides(buffer_index);
    size_t offset = _strides[buffer_index];

    _elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE, 0, offset, buffer_index, divisor});

    _strides[buffer_index] += get_sizeof(GL_UNSIGNED_INT) * count;
}

template <>
inline void VertexLayout::push<uint8_t>(size_t count, size_t buffer_index, size_t divisor) {
    ensure_strides(buffer_index);
    size_t offset = _strides[buffer_index];

    _elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE, 0, offset, buffer_index, divisor});

    _strides[buffer_index] += get_sizeof(GL_UNSIGNED_BYTE) * count;
}

inline void VertexLayout::finalize() {
    for (auto& e : _elements) e.stride = _strides[e.buffer_index];
}

inline const std::vector<VertexElement>& VertexLayout::elements() const { return _elements; }

inline void VertexLayout::ensure_strides(size_t buffer_index) {
    if (buffer_index >= _strides.size()) _strides.resize(buffer_index + 1, 0);
}

inline size_t VertexLayout::get_sizeof(GLenum type) {
    // clang-format off
    switch (type) {
        case GL_FLOAT        : return 4;
        case GL_UNSIGNED_INT : return 4;
        case GL_UNSIGNED_BYTE: return 1;
    }
    // clang-format on
    return 0;
}