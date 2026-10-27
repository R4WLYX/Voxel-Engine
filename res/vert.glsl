#version 430 core

layout(location = 0) in uint data;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec2 v_uv;

void main() {
    v_uv = uv;
    gl_Position = u_proj * u_view * u_model * vec4(pos, 1.0);
}