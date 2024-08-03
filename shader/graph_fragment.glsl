#version 450

layout(location = 0) in vec3 object_color;
layout(location = 1) in vec3 object_normal;

// out
layout(location = 0) out vec4 final_color;

const vec3 light_color = vec3(1.0, 1.0, 1.0);
const vec3 light_direction = vec3(-1.0f, -1.0f, -1.0f);

void main() {
    float diff = max(dot(object_normal, light_direction), 0.0f);

    vec3 diffuse = diff * light_color;
    vec3 light   = diffuse * object_color;

    final_color = vec4(light, 1.0f);
}
