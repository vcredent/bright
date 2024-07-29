#version 450

layout(location = 1) in vec4 normal;

// out
layout(location = 0) out vec4 final_color;

vec3 light_dir = vec3(-1.0f, -1.0f, -1.0f);

void main() {
    float color = clamp(0.0f, dot(light_dir, normal), 1.0f);
    final_color = vec4(color, color, color, 1.0f);
}
