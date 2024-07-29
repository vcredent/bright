#version 450

layout(location = 0) in vec3 posiiton;
layout(location = 2) in vec2 texcoord;
layout(location = 1) in vec3 normal;

// out
layout(location = 0) out vec4 final_color;

vec3 light_dir = vec3(-1.0f, -1.0f, -1.0f);
vec3 light_color = vec3(1.0f, 1.0f, 1.0f);

void main() {
    // 计算法线与光源方向的夹角
    vec3 norm = normalize(normal);
    vec3 direction = normalize(-light_dir);

    // 简单的光照计算
    float diff = max(dot(norm, direction), 0.0);

    // 设置颜色（这里只用了光照强度，简化了颜色计算）
    vec3 color = diff * light_color;

    final_color = vec4(color, 1.0f);
}
