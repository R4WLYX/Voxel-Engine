#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "mesh.hpp"
#include "vertex.hpp"

class MeshLoader {
  public:
    Mesh load_obj(const std::string&) const;

  private:
    struct ObjVertex {
        int pos, uv, norm;

        bool operator==(const ObjVertex& other) const {
            return pos == other.pos && uv == other.uv && norm == other.norm;
        }
    };

    struct ObjVertexHash {
        std::size_t operator()(const ObjVertex& v) const {
            std::size_t h = std::hash<int>{}(v.pos);
            h ^= std::hash<int>{}(v.uv) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(v.norm) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    glm::vec3 to_floats(const std::string&) const;
};

inline Mesh MeshLoader::load_obj(const std::string& path) const {
    std::ifstream file(path);
    std::string line;

    Vertex_SoA vertices;
    std::vector<unsigned int> indices;

    Vertex_SoA temp;
    std::unordered_map<ObjVertex, unsigned int, ObjVertexHash> vertex_map;

    while (std::getline(file, line)) {
        if (line.starts_with("#")) {
            continue;
        }
        else if (line.starts_with("v ")) {
            temp.pos.push_back(to_floats(line.substr(3)));
        }
        else if (line.starts_with("vn ")) {
            temp.norm.push_back(to_floats(line.substr(3)));
        }
        else if (line.starts_with("vt ")) {
            auto value = to_floats(line.substr(3));
            temp.uv.push_back({value[0], value[1]});
        }
        else if (line.starts_with("f ")) {
            std::istringstream ss(line.substr(2));

            std::string token;
            std::vector<unsigned int> face;

            while (ss >> token) {
                int pos = 0;
                int uv = -1;
                int norm = -1;

                std::stringstream tkn_ss(token);
                std::string str;

                std::getline(tkn_ss, str, '/');
                pos = std::stoi(str) - 1;
                std::getline(tkn_ss, str, '/');
                uv = std::stoi(str) - 1;
                std::getline(tkn_ss, str, '/');
                norm = std::stoi(str) - 1;

                ObjVertex key{pos, uv, norm};
                unsigned int index = static_cast<unsigned int>(vertices.pos.size());

                auto [it, inserted] = vertex_map.try_emplace(key, index);

                if (inserted) {
                    vertices.pos.push_back(temp.pos[pos]);
                    vertices.uv.push_back(temp.uv[uv]);
                    vertices.norm.push_back(temp.norm[norm]);
                }

                face.push_back(it->second);
            }

            for (size_t i = 1; i + 1 < face.size(); ++i) {
                indices.push_back(face[0]);
                indices.push_back(face[i]);
                indices.push_back(face[i + 1]);
            }
        }
    }

    if (!vertices.pos.empty()) {
        glm::vec3 min_pos{vertices.pos[0]};
        glm::vec3 max_pos{vertices.pos[0]};

        for (const auto& pos : vertices.pos) {
            min_pos = glm::min(min_pos, pos);
            max_pos = glm::max(max_pos, pos);
        }

        const glm::vec3 center = (min_pos + max_pos) * 0.5f;

        for (auto& pos : vertices.pos) {
            pos -= center;
        }
    }

    VertexBuffers vertex_buffers;
    vertex_buffers.push(vertices.pos);
    vertex_buffers.push(vertices.norm);
    vertex_buffers.push(vertices.uv);

    VertexLayout layout;
    layout.push<float>(3, 0);
    layout.push<float>(3, 1);
    layout.push<float>(2, 2);
    layout.finalize();

    MeshData mesh_data{vertex_buffers, indices, layout};
    Mesh mesh{mesh_data};

    return mesh;
}

inline glm::vec3 MeshLoader::to_floats(const std::string& str) const {
    glm::vec3 result{};
    std::istringstream ss(str);
    ss >> result.x >> result.y >> result.z;
    return result;
}