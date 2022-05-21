#version 330 core
layout (location = 0) in vec3 in_vertex_pos;

uniform mat4 camera;
uniform mat4 transform;

out vec3 vout_world_pos;

void main() {
    vec4 world_pos = transform * vec4(in_vertex_pos, 1.0);
    gl_Position = camera * world_pos;
    vout_world_pos = world_pos.xyz;
}