#version 430 core

in vec2 v_uv;

out vec4 frag_colour;

void main() {
    frag_colour = vec4(v_uv, 0.0, 1.0);
}