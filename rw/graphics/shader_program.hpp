#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

class ShaderProgram {
  public:
    ShaderProgram();
    ~ShaderProgram() = default;

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void attach_shader(Shader&);
    void compile();

    void bind();

    template <typename... T>
    void set_uniform(const std::string&, T...);

  private:
    GLuint _program_handle;
    std::unordered_map<std::string, int> _uniform_cache;

    int get_uniform_location(const std::string&);
};

inline ShaderProgram::ShaderProgram() { _program_handle = glCreateProgram(); }

inline void ShaderProgram::attach_shader(Shader& shader) { shader.attach(_program_handle); }

inline void ShaderProgram::compile() {
    glLinkProgram(_program_handle);
    
    int success;
    glGetProgramiv(_program_handle, GL_LINK_STATUS, &success);

    if (!success) {
        char info[512];
        glGetProgramInfoLog(_program_handle, 512, nullptr, info);
        throw std::runtime_error("Error: Shader linking failed: " + std::string(info));
    }
}


inline void ShaderProgram::bind() { glUseProgram(_program_handle); }

inline int ShaderProgram::get_uniform_location(const std::string& name) {
    auto it = _uniform_cache.find(name);
    if (it != _uniform_cache.end()) return it->second;

    int location = glGetUniformLocation(_program_handle, name.c_str());
    if (location == -1) std::cerr << "Warning: uniform '" << name << "' not found\n";

    _uniform_cache[name] = location;
    return location;
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, bool x) {
    int location = get_uniform_location(name);
    glUniform1i(location, x ? 1 : 0);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, int x) {
    int location = get_uniform_location(name);
    glUniform1i(location, x);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, float x) {
    int location = get_uniform_location(name);
    glUniform1f(location, x);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::ivec2 vec) {
    int location = get_uniform_location(name);
    glUniform2i(location, vec.x, vec.y);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::ivec3 vec) {
    int location = get_uniform_location(name);
    glUniform3i(location, vec.x, vec.y, vec.z);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::ivec4 vec) {
    int location = get_uniform_location(name);
    glUniform4i(location, vec.x, vec.y, vec.z, vec.w);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::vec2 vec) {
    int location = get_uniform_location(name);
    glUniform2f(location, vec.x, vec.y);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::vec3 vec) {
    int location = get_uniform_location(name);
    glUniform3f(location, vec.x, vec.y, vec.z);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::vec4 vec) {
    int location = get_uniform_location(name);
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::mat2 mat) {
    int location = get_uniform_location(name);
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::mat3 mat) {
    int location = get_uniform_location(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

template <>
inline void ShaderProgram::set_uniform(const std::string& name, glm::mat4 mat) {
    int location = get_uniform_location(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}