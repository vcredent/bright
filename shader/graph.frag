#version 450

layout(location = 0) in vec3 object_color;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec3 world_position;
layout(location = 3) in vec3 camera_pos;

layout(set = 0, binding = 1) uniform DirectionalLight {
    vec3 direction;
    float intensity;
    float specular_exponent;
    float ambient;
    vec3 color;
    vec3 specular_color;
} light;

// out
layout(location = 0) out vec4 final_color;

void main()
{
    // 计算光源方向
    vec3 light_dir = normalize(light.direction);

    // 漫反射
    vec3 normal = normalize(world_normal);
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = vec3(0.5f) * diff;

    // 高光
    vec3 view_dir = normalize(camera_pos - world_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), light.specular_exponent);
    vec3 specular = light.specular_color * spec;

    // 合成光照
    vec3 result = vec3(light.ambient) + diffuse + specular;

    final_color = vec4(result * object_color, 1.0f);
}
