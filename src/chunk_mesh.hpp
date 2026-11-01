#include <array>
#include <bit>

#include <rw/graphics/mesh.hpp>

class ChunkMesh {
  public:
    ChunkMesh(const std::vector<uint8_t>& voxels);

    void draw() const;

  private:
    enum class FaceAxis : uint8_t { X, Y, Z };

    static constexpr size_t chunk_size = 32;
    static inline std::vector<uint32_t> indices{0, 1, 2, 3};

    std::array<Mesh, 6> _meshes;

    void
    build_layer(const std::vector<uint8_t>& voxels, FaceAxis axis, size_t slice, std::vector<uint32_t>& layer) const;

    std::vector<uint32_t>
    greedy_mesher(std::vector<uint32_t>& layer, uint32_t slice, std::vector<uint32_t>& quads) const;
};

inline ChunkMesh::ChunkMesh(const std::vector<uint8_t>& voxels) {
    std::array<std::vector<uint32_t>, 6> quads;

    std::vector<uint32_t> current(chunk_size);
    std::vector<uint32_t> previous(chunk_size);
    std::vector<uint32_t> next(chunk_size);

    std::vector<uint32_t> positive(chunk_size);
    std::vector<uint32_t> negative(chunk_size);

    VertexLayout layout;
    layout.push<uint32_t>(1, 0, 1);
    layout.finalize();

    for (size_t axis = 0; axis < 3; ++axis) {
        for (size_t slice = 0; slice < chunk_size; ++slice) {
            build_layer(voxels, FaceAxis(axis), slice, current);

            if (slice > 0) build_layer(voxels, FaceAxis(axis), slice - 1, previous);
            else std::fill(previous.begin(), previous.end(), 0);

            if (slice + 1 < chunk_size) build_layer(voxels, FaceAxis(axis), slice + 1, next);
            else std::fill(next.begin(), next.end(), 0);

            for (size_t i = 0; i < chunk_size; ++i) {
                positive[i] = current[i] & ~next[i];
                negative[i] = current[i] & ~previous[i];
            }

            greedy_mesher(positive, slice, quads[axis * 2]);
            greedy_mesher(negative, slice, quads[axis * 2 + 1]);
        }
    }

    for (size_t i = 0; i < _meshes.size(); ++i) {
        VertexBuffers buffers;
        buffers.push(quads[i]);

        _meshes[i] = Mesh{{buffers, indices, layout}};
        _meshes[i].instance_count = quads[i].size();
        _meshes[i].primitive = GL_TRIANGLE_STRIP;
    }
}

inline void ChunkMesh::draw() const {
    for (const auto& m : _meshes) m.draw();
}

inline void ChunkMesh::build_layer(
    const std::vector<uint8_t>& voxels, FaceAxis axis, size_t slice, std::vector<uint32_t>& layer
) const {
    std::fill(layer.begin(), layer.end(), 0);

    for (size_t u = 0; u < chunk_size; ++u) {
        for (size_t v = 0; v < chunk_size; ++v) {
            size_t x, y, z;

            switch (axis) {
            case FaceAxis::X:
                x = slice;
                y = u;
                z = v;
                break;

            case FaceAxis::Y:
                x = u;
                y = slice;
                z = v;
                break;

            case FaceAxis::Z:
                x = u;
                y = v;
                z = slice;
                break;
            }

            size_t index = x + y * chunk_size + z * chunk_size * chunk_size;
            layer[u] |= uint32_t(voxels[index] != 0) << v;
        }
    }
}

inline std::vector<uint32_t>
ChunkMesh::greedy_mesher(std::vector<uint32_t>& layer, uint32_t slice, std::vector<uint32_t>& quads) const {
    for (size_t x = 0; x < chunk_size; x++) {
        for (size_t y = 0; y < chunk_size;) {
            y += std::countr_zero(layer[x] >> y);
            if (y >= chunk_size) break;

            size_t h = std::countr_one(layer[x] >> y);

            uint32_t h_as_mask = (h >= chunk_size) ? ~0u : (1u << h) - 1u;
            uint32_t mask = h_as_mask << y;

            size_t w = 1;

            for (; x + w < chunk_size; ++w) {
                uint32_t next_row_h = (layer[x + w] >> y) & h_as_mask;
                if (next_row_h != h_as_mask) break;
                layer[x + w] &= ~mask;
            }

            uint32_t quad = 0;
            quad |= ((w - 1) & 31u) << 20u;
            quad |= ((h - 1) & 31u) << 15u;
            quad |= (x & 31u) << 10u;
            quad |= (y & 31u) << 5u;
            quad |= slice & 31u;

            quads.push_back(quad);

            y += h;
        }
    }

    return quads;
}