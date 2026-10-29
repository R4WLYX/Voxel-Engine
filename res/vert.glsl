#version 430 core

layout(location = 0) in uint quad;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec2 v_uv;

const vec2 corners[4] = vec2[4](
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0)
);

void main() {
    uint w = ((quad >> 20u) & 31u) + 1;
    uint h = ((quad >> 15u) & 31u) + 1;
    uint x = (quad >> 10u) & 31u;
    uint y = (quad >> 5u) & 31u;
    uint z = quad & 31u;

    vec2 corner = corners[gl_VertexID];

    vec3 pos = vec3(
        float(x) + corner.x * float(w),
        float(y) + corner.y * float(h),
        float(z)
    );

    gl_Position = u_proj * u_view * u_model * vec4(pos, 1.0);
    v_uv = corner;
}