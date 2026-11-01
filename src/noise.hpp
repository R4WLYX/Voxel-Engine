#include <random>
#include <vector>

#include <glm/glm.hpp>

class Perlin3D {
  public:
    Perlin3D(unsigned int seed = std::random_device{}());

    float sample(float x, float y, float z);

  private:
    static constexpr float s = 0.70710678118f;

    inline static const std::vector<glm::vec3> _gradients{
        {s, s, 0.0f},
        {-s, s, 0.0f},
        {s, -s, 0.0f},
        {-s, -s, 0.0f},

        {s, 0.0f, s},
        {-s, 0.0f, s},
        {s, 0.0f, -s},
        {-s, 0.0f, -s},

        {0.0f, s, s},
        {0.0f, -s, s},
        {0.0f, s, -s},
        {0.0f, -s, -s}
    };

    unsigned int _seed;

    uint32_t hash(int x, int y, int z, uint32_t seed) const;

    glm::vec3 gradient_at(int x, int y, int z) const;

    float fade(float t) const;

    float lerp(float a, float b, float t) const;
};

inline Perlin3D::Perlin3D(unsigned int seed) : _seed(seed) {}

inline float Perlin3D::sample(float x, float y, float z) {
    int ix = std::floor(x);
    int iy = std::floor(y);
    int iz = std::floor(z);

    glm::vec3 g0 = gradient_at(ix, iy, iz);
    glm::vec3 g1 = gradient_at(ix, iy, iz + 1);
    glm::vec3 g2 = gradient_at(ix, iy + 1, iz);
    glm::vec3 g3 = gradient_at(ix, iy + 1, iz + 1);

    glm::vec3 g4 = gradient_at(ix + 1, iy, iz);
    glm::vec3 g5 = gradient_at(ix + 1, iy, iz + 1);
    glm::vec3 g6 = gradient_at(ix + 1, iy + 1, iz);
    glm::vec3 g7 = gradient_at(ix + 1, iy + 1, iz + 1);

    float tx = x - ix;
    float ty = y - iy;
    float tz = z - iz;

    float d0 = glm::dot(g0, glm::vec3(tx, ty, tz));
    float d1 = glm::dot(g1, glm::vec3(tx, ty, tz - 1));
    float d2 = glm::dot(g2, glm::vec3(tx, ty - 1, tz));
    float d3 = glm::dot(g3, glm::vec3(tx, ty - 1, tz - 1));

    float d4 = glm::dot(g4, glm::vec3(tx - 1, ty, tz));
    float d5 = glm::dot(g5, glm::vec3(tx - 1, ty, tz - 1));
    float d6 = glm::dot(g6, glm::vec3(tx - 1, ty - 1, tz));
    float d7 = glm::dot(g7, glm::vec3(tx - 1, ty - 1, tz - 1));

    float u = fade(tx);
    float v = fade(ty);
    float w = fade(tz);

    float x00 = lerp(d0, d4, u);
    float x01 = lerp(d1, d5, u);
    float x10 = lerp(d2, d6, u);
    float x11 = lerp(d3, d7, u);

    float y0 = lerp(x00, x10, v);
    float y1 = lerp(x01, x11, v);

    return lerp(y0, y1, w);
}

inline uint32_t Perlin3D::hash(int x, int y, int z, uint32_t seed) const {
    uint32_t h = seed;

    h ^= static_cast<uint32_t>(x) * 374761393u;
    h ^= static_cast<uint32_t>(y) * 668265263u;
    h ^= static_cast<uint32_t>(z) * 2147483647u;

    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;

    return h;
}

inline glm::vec3 Perlin3D::gradient_at(int x, int y, int z) const { return _gradients[hash(x, y, z, _seed) % 12]; }

inline float Perlin3D::fade(float t) const { return t * t * t * (t * (t * 6 - 15) + 10); }

inline float Perlin3D::lerp(float a, float b, float t) const { return a + t * (b - a); }