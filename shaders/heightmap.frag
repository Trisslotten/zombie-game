#version 330 core
out vec4 out_frag_color;

in vec2 vout_uv;
in vec3 vout_world_pos;
in vec3 vout_normal;

const float ambient = 0.2;
const vec3 light_color = vec3(1);
const vec3 light_dir = normalize(vec3(2,3,1));

void main() {
    const vec3 heightmap_color = vec3(0.5, 1, 0.5);

    float diffuse = 0.5 * max(dot(vout_normal, light_dir), 0.0);

    float asd = ambient * (max(dot(vout_normal, vec3(0,0,1)), 0.0) + 0.5);

    float lighting = 0;
    lighting += asd;
    lighting += diffuse;

    // out_frag_color = vec4(lighting * light_color * heightmap_color, 1.0);

    out_frag_color = vec4(fract(vout_world_pos), 1.0);
}
