#version 330 core
layout (location = 0) in vec2 in_vertex_pos;

uniform mat4 camera;

out vec2 vout_uv;
out vec3 vout_world_pos;
out vec3 vout_normal;

uniform sampler2D heightmap;

void main() {
    const float lod = 0.0;
    float size = textureSize(heightmap, 0).x;
    float height_scale = 0.3;
    float height = height_scale*textureLod(heightmap, in_vertex_pos, lod).x;
    float heightx = height_scale*textureLod(heightmap, in_vertex_pos + vec2(1./size, 0), lod).x;
    float heighty = height_scale*textureLod(heightmap, in_vertex_pos + vec2(0, 1./size), lod).x;

    vec3 x = vec3(0.1, 0, heightx - height);
    vec3 y = vec3(0, 0.1, heighty - height);
    vout_normal = normalize(cross(x, y));
    vout_world_pos = 100.0*vec3(50.0*in_vertex_pos - 25.0, height);
    vout_uv = in_vertex_pos;

    gl_Position = camera * vec4(vout_world_pos, 1.0);
}