#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include <rw/graphics/engine.hpp>
#include <rw/graphics/window.hpp>
#include <rw/graphics/mesh.hpp>
#include <rw/graphics/material.hpp>

#include <glm/ext/matrix_transform.hpp>

int main() {
    try {
        // clang-format off
        WindowContext context{
            .title = "Hello Cube!",

            .width = 1920,
            .height = 1080,

            .decorated = false,

            .opengl = {
                .major = 4,
                .minor = 3,
                .forward_compatible = true,
                .profile = GLProfile::Core
            }
        };
        // clang-format on

        Engine engine(context);

        Window& window = engine.window();
        // window.set_vsync(true);

        // clang-format off
        std::vector<float>vertices{
            // FRONT (+Z)
            -0.5,-0.5, 0.5, 0, 0, 1, 0, 0,
                0.5,-0.5, 0.5, 0, 0, 1, 1, 0,
                0.5, 0.5, 0.5, 0, 0, 1, 1, 1,
            -0.5, 0.5, 0.5, 0, 0, 1, 0, 1,

            // BACK (-Z)
            -0.5,-0.5,-0.5, 0, 0,-1, 1, 0,
            -0.5, 0.5,-0.5, 0, 0,-1, 1, 1,
                0.5, 0.5,-0.5, 0, 0,-1, 0, 1,
                0.5,-0.5,-0.5, 0, 0,-1, 0, 0,

            // LEFT (-X)
            -0.5,-0.5,-0.5,-1, 0, 0, 0, 0,
            -0.5,-0.5, 0.5,-1, 0, 0, 1, 0,
            -0.5, 0.5, 0.5,-1, 0, 0, 1, 1,
            -0.5, 0.5,-0.5,-1, 0, 0, 0, 1,

            // RIGHT (+X)
                0.5,-0.5,-0.5, 1, 0, 0, 1, 0,
                0.5, 0.5,-0.5, 1, 0, 0, 1, 1,
                0.5, 0.5, 0.5, 1, 0, 0, 0, 1,
                0.5,-0.5, 0.5, 1, 0, 0, 0, 0,

            // TOP (+Y)
            -0.5, 0.5,-0.5, 0, 1, 0, 0, 1,
            -0.5, 0.5, 0.5, 0, 1, 0, 0, 0,
                0.5, 0.5, 0.5, 0, 1, 0, 1, 0,
                0.5, 0.5,-0.5, 0, 1, 0, 1, 1,

            // BOTTOM (-Y)
            -0.5,-0.5,-0.5, 0,-1, 0, 0, 0,
                0.5,-0.5,-0.5, 0,-1, 0, 1, 0,
                0.5,-0.5, 0.5, 0,-1, 0, 1, 1,
            -0.5,-0.5, 0.5, 0,-1, 0, 0, 1,
        };

        std::vector<unsigned int> indices{
            // FRONT (+Z)
            0,  1,  2,
            2,  3,  0,

            // BACK (-Z)
            4,  5,  6,
            6,  7,  4,

            // LEFT (-X)
            8,  9, 10,
            10, 11,  8,

            // RIGHT (+X)
            12, 13, 14,
            14, 15, 12,

            // TOP (+Y)
            16, 17, 18,
            18, 19, 16,

            // BOTTOM (-Y)
            20, 21, 22,
            22, 23, 20
        };
        // clang-format on

        constexpr size_t chunk_size = 32;
        constexpr float cutoff = 0.5f;

        auto grid_index = [=](size_t x, size_t y, size_t z) {
            return x + chunk_size * (y + chunk_size * z);
        };

        std::vector<uint8_t> occupied(chunk_size * chunk_size * chunk_size, 0);

        std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<float> prob(0.0f, 1.0f);

        size_t voxel_count = 0;

        for (size_t z = 0; z < chunk_size; ++z)
            for (size_t y = 0; y < chunk_size; ++y)
                for (size_t x = 0; x < chunk_size; ++x)
                    if (prob(rng) <= cutoff) {
                        occupied[grid_index(x, y, z)] = 1;
                        ++voxel_count;
                    }

        // clang-format off
        constexpr int dirs[6][3] = {
            {0, 0, 1}, {0, 0, -1},
            {-1, 0, 0}, {1, 0, 0},
            {0, 1, 0}, {0, -1, 0}
        };
        // clang-format on

        std::vector<float> mesh_vertices;
        std::vector<unsigned int> mesh_indices;

        constexpr float offset = (chunk_size - 1) / 2.0f;

        auto T0 = std::chrono::high_resolution_clock::now();

        for (size_t z = 0; z < chunk_size; ++z)
            for (size_t y = 0; y < chunk_size; ++y)
                for (size_t x = 0; x < chunk_size; ++x) {
                    if (!occupied[grid_index(x, y, z)]) continue;

                    for (size_t face = 0; face < 6; ++face) {
                        int nx = int(x) + dirs[face][0];
                        int ny = int(y) + dirs[face][1];
                        int nz = int(z) + dirs[face][2];

                        bool in_bounds = nx >= 0 && nx < int(chunk_size) && ny >= 0 && ny < int(chunk_size) && nz >= 0
                                         && nz < int(chunk_size);

                        bool neighbor_exists = false;

                        if (in_bounds) neighbor_exists = occupied[grid_index(size_t(nx), size_t(ny), size_t(nz))];

                        if (neighbor_exists) continue;

                        unsigned int base = mesh_vertices.size() / 8;

                        size_t face_start = face * 4 * 8;

                        for (size_t v = 0; v < 4; ++v) {
                            size_t start = face_start + v * 8;

                            mesh_vertices.push_back(vertices[start + 0] + float(x) - offset);
                            mesh_vertices.push_back(vertices[start + 1] + float(y) - offset);
                            mesh_vertices.push_back(vertices[start + 2] + float(z) - offset);

                            for (size_t k = 3; k < 8; ++k) mesh_vertices.push_back(vertices[start + k]);
                        }

                        mesh_indices.insert(mesh_indices.end(), {base, base + 1, base + 2, base + 2, base + 3, base});
                    }
                }

        auto T1 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T0);

        std::cout << "Meshing: " << duration.count() << "ms\n";
        std::cout << "Voxels: " << voxel_count << '\n';
        std::cout << "Faces: " << mesh_vertices.size() / (8 * 4) << '\n';
        std::cout << "Polygons: " << mesh_indices.size() / 3 << '\n';

        VertexBuffers vertex_buffers;
        vertex_buffers.push(mesh_vertices);

        VertexLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);
        layout.finalize();

        MeshData mesh_data{vertex_buffers, mesh_indices, layout};

        Mesh mesh(mesh_data);

        Shader vertex_shader(ShaderType::Vertex, "../res/vert.glsl");
        Shader fragment_shader(ShaderType::Fragment, "../res/frag.glsl");

        ShaderProgram shader;
        shader.attach_shader(vertex_shader);
        shader.attach_shader(fragment_shader);
        shader.compile();

        Material material(shader);

        glm::mat4 proj = glm::perspective(glm::radians(70.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
        glm::mat4 view =
            glm::lookAt(glm::vec3{0, 0, -float(chunk_size) * 1.25f}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});
        glm::mat4 model = glm::translate(glm::mat4(1.0), {0, 0, 0});

        material.set_uniform("u_proj", proj);
        material.set_uniform("u_view", view);
        material.set_uniform("u_model", model);

        while (!window.should_close()) {
            window.poll_events();
            window.clear();

            material.bind();
            mesh.draw();

            window.swap_buffers();
        }
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}