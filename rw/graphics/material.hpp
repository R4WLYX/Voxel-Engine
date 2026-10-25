#pragma once

#include <string>
#include <unordered_map>
#include <variant>

#include <glm/glm.hpp>

#include "shader_program.hpp"
#include "texture.hpp"

class Material {
  public:
    Material(ShaderProgram& shader) : _shader_program(shader) {}
    ~Material() = default;

    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    // clang-format off
    using Uniform = std::variant<
        bool, int, float,
        glm::ivec2, glm::ivec3, glm::ivec4,
        glm:: vec2, glm:: vec3, glm:: vec4,
        glm:: mat2, glm:: mat3, glm:: mat4
    >;
    // clang-format on

    struct UniformHandle {
        Uniform value;
        bool dirty = true;
    };

    std::unordered_map<std::string, UniformHandle> uniforms;

    struct TextureHandle {
        Texture texture;
        size_t id = 0;
    };

    std::unordered_map<std::string, TextureHandle> textures;

    void set_uniform(const std::string&, const Uniform&);

    void bind();

  private:
    ShaderProgram& _shader_program;
};

inline void Material::set_uniform(const std::string& name, const Uniform& value) {
    UniformHandle& uniform = uniforms[name];
    uniform.value = value;
    uniform.dirty = true;
}

inline void Material::bind() {
    _shader_program.bind();
    for (auto& [name, uniform] : uniforms)
        if (uniform.dirty) {
            std::visit([&](auto&& v) { _shader_program.set_uniform(name, v); }, uniform.value);
            uniform.dirty = false;
        }
}