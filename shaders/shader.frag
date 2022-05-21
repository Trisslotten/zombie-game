#version 330 core
out vec4 out_frag_color;

in vec3 vout_world_pos;

uniform vec3 color;

const float ambient = 0.1;
const vec3 light_color = vec3(1);
const vec3 light_dir = normalize(vec3(2,3,1));

void main() {
    vec3 dx = dFdx(vout_world_pos);
    vec3 dy = dFdy(vout_world_pos);
    vec3 normal = normalize(cross(dx, dy));

    float diffuse = max(dot(normal, light_dir), 0.0);

    float lighting = 0;
    lighting += ambient;
    lighting += diffuse;
    
    out_frag_color = vec4(lighting * light_color * color, 1.0);
} 