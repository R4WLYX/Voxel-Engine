#include <bit>
#include <cstdint>
#include <random>
#include <vector>

#include <rw/graphics/mesh.hpp>

class Chunk {
  public:
    Chunk(size_t x, size_t y, size_t z);

    void draw() const;

  private:
    static constexpr size_t chunk_size = 32;
    static inline std::vector<uint32_t> indices{0, 1, 2, 3};

    std::vector<uint32_t> _occupied;

    MeshData _mesh_data;
    Mesh _mesh;

    std::vector<uint32_t> greedy_mesher(std::vector<uint32_t>& layer, uint32_t z) const;
};

inline Chunk::Chunk(size_t x, size_t y, size_t z) {
    _occupied.resize(chunk_size * chunk_size, 0);

    std::uniform_real_distribution<float> dist{0.0f, 1.0f};
    std::mt19937 rng{std::random_device{}()};

    for (uint32_t& strip : _occupied) {
        for (size_t i = 0; i < chunk_size; i++) {
            strip <<= 1;
            strip |= dist(rng) < 0.1f;
        }
    }

    std::vector<uint32_t> quads{};
    for (size_t z = 0; z < chunk_size; z++) {
        auto start = _occupied.begin() + z * chunk_size;
        std::vector<uint32_t> layer(start, start + chunk_size);
        std::vector<uint32_t> new_quads = greedy_mesher(layer, z);
        quads.reserve(quads.size() + new_quads.size());
        quads.insert(quads.end(), new_quads.begin(), new_quads.end());
    }

    VertexBuffers vertex_buffers;
    vertex_buffers.push(quads);

    VertexLayout layout;
    layout.push<uint32_t>(1, 0, 1);
    layout.finalize();

    _mesh_data = MeshData{vertex_buffers, indices, layout};

    _mesh = Mesh{_mesh_data};
    _mesh.instance_count = quads.size();
    _mesh.primitive = GL_TRIANGLE_STRIP;
}

inline void Chunk::draw() const {
    _mesh.draw();
}

inline std::vector<uint32_t> Chunk::greedy_mesher(std::vector<uint32_t>& layer, uint32_t z) const {
    std::vector<uint32_t> quads{};

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
            quad |= z & 31u;

            quads.push_back(quad);

            y += h;
        }
    }

    return quads;
}