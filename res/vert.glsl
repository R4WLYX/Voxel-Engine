#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in uint data;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec2 v_uv;

struct Quad {
   vec3 position;
   vec2 dimensions;
};

void read_quad_data(in uint data, out Quad quad) {
    
}

void main() {
    gl_Position = u_proj * u_view * u_model * vec4(pos, 1.0);
    v_uv = uv;
    quad
}