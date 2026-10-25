#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <vector>

#include <glad/glad.h>

struct VertexBufferData {
    std::vector<std::byte> data;
    GLenum usage = GL_STATIC_DRAW;

    template <typename T, size_t N>
    void set(const T (&)[N]);

    template <typename T, size_t N>
    void set(const std::array<T, N>&);

    template <typename T>
    void set(const std::vector<T>&);
};

struct VertexBuffers {
    std::vector<VertexBufferData> buffers;

    template <typename T, size_t N>
    void push(const T (&)[N]);

    template <typename T, size_t N>
    void push(const std::array<T, N>&);

    template <typename T>
    void push(const std::vector<T>&);
};

template <typename T, size_t N>
inline void VertexBufferData::set(const T (&elements)[N]) {
    data.resize(N * sizeof(T));
    std::memcpy(data.data(), elements, data.size());
}

template <typename T, size_t N>
inline void VertexBufferData::set(const std::array<T, N>& elements) {
    data.resize(N * sizeof(T));
    std::memcpy(data.data(), elements.data(), data.size());
}

template <typename T>
inline void VertexBufferData::set(const std::vector<T>& elements) {
    data.resize(elements.size() * sizeof(T));
    std::memcpy(data.data(), elements.data(), data.size());
}

template <typename T, size_t N>
inline void VertexBuffers::push(const T (&elements)[N]) {
    VertexBufferData buffer;
    buffer.set(elements);
    buffers.push_back(std::move(buffer));
}

template <typename T, size_t N>
inline void VertexBuffers::push(const std::array<T, N>& elements) {
    VertexBufferData buffer;
    buffer.set(elements);
    buffers.push_back(std::move(buffer));
}

template <typename T>
inline void VertexBuffers::push(const std::vector<T>& elements) {
    VertexBufferData buffer;
    buffer.set(elements);
    buffers.push_back(std::move(buffer));
}