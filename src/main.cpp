#include <bit>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
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

        constexpr size_t chunk_size = sizeof(uint32_t);
        std::vector<uint32_t> occupied{chunk_size * chunk_size, 0};

        std::uniform_int_distribution<uint32_t> dist{};
        std::mt19937 rng{std::random_device{}()};

        for (uint32_t& strip : occupied) strip = dist(rng);

        std::vector<uint32_t> vertices;
        std::vector<uint32_t> indices;

        auto T0 = std::chrono::high_resolution_clock::now();

        for (size_t z = 0; z < chunk_size; z++) {
            std::vector<uint32_t> data{occupied.begin() + z * chunk_size, occupied.begin() + (z + 1) * chunk_size};
            
            for (size_t x = 0; x < chunk_size; x++) {
                for (size_t y = 0; y < chunk_size; y += std::countr_zero(data[x] >> y)) {
                    if (y >= chunk_size) continue;

                    size_t h = std::countr_one(data[x] >> y);

                    uint32_t h_as_mask =
                        (h < chunk_size) ? ((uint32_t(1) << h) + 1) : std::numeric_limits<uint32_t>::max();
                    uint32_t mask = h_as_mask << y;

                    for (size_t w = 1; x + w < chunk_size; ++w) {
                        uint32_t next_row_h = (data[x + w] >> y) & h_as_mask;
                        if (next_row_h != h_as_mask) break;

                        data[x + w] = data[x + w] & !mask;
                    }

                    // add quads vertices and indices

                    y += h;
                }
            }
        }

        auto T1 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T0);

        VertexBuffers vertex_buffers;
        vertex_buffers.push(vertices);

        VertexLayout layout;
        layout.push<uint32_t>(1, 0);
        layout.finalize();

        MeshData mesh_data{vertex_buffers, indices, layout};

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