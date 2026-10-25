#pragma once

#include <vector>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    float uv[2];
};

struct Vertex_SoA {
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> norm;
    std::vector<glm::vec2> uv;
};