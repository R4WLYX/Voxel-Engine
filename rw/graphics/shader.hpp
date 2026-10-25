#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <glad/glad.h>

enum class ShaderType { Fragment, Vertex, Geometry };

class Shader {
  public:
    Shader(ShaderType, const std::filesystem::path&);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void attach(GLuint);

  private:
    GLuint _shader_handle;
    ShaderType _type;
};

inline Shader::Shader(ShaderType type, const std::filesystem::path& file_path) {
    // clang-format off
    switch (type) {
        case ShaderType::Fragment: _shader_handle = glCreateShader(GL_FRAGMENT_SHADER); break;
        case ShaderType::Vertex  : _shader_handle = glCreateShader(GL_VERTEX_SHADER)  ; break;
        case ShaderType::Geometry: _shader_handle = glCreateShader(GL_GEOMETRY_SHADER); break;
        default: throw std::runtime_error("Error: Unkown shader type!");
    }
    // clang-format on

    std::ifstream file(file_path, std::ios_base::binary);
    std::ostringstream ss;

    ss << file.rdbuf();
    file.close();

    std::string source_string = ss.str();
    const char* source = source_string.c_str();

    glShaderSource(_shader_handle, 1, &source, NULL);
    glCompileShader(_shader_handle);

    int success;
    glGetShaderiv(_shader_handle, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        char info[512];
        glGetShaderInfoLog(_shader_handle, 512, nullptr, info);
        throw std::runtime_error("Error: Shader compilation failed: " + std::string(info));
    }
}

inline Shader::~Shader() { glDeleteShader(_shader_handle); }

inline void Shader::attach(GLuint program_handle) { glAttachShader(program_handle, _shader_handle); }