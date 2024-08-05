#version 450

layout(location = 0) in vec3 object_color;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec3 world_position;
layout(location = 3) in vec3 camera_pos;

// out
layout(location = 0) out vec4 final_color;

const vec3 DIRECTION_TO_LIGHT = vec3(-1.0f, -1.0f, -1.0f); // 平行光
const vec3 LIGHT_COLOR = vec3(1.0f, 1.0f, 1.0f); // 光源颜色
const float LIGHT_INTSENTIY = 1.0f; // 光源强度
const float SPECULAR_EXPONENT = 32.0f; // 高光指数（控制镜面高光的硬度）
const vec3 SPECULAR_COLOR = vec3(1.0f, 1.0f, 1.0f); // 高光颜色
const vec3 AMBIENT = vec3(0.2f, 0.2f, 0.2f); // 环境光

void main()
{
    // 计算光源方向
    vec3 light_dir = normalize(DIRECTION_TO_LIGHT);

    // 漫反射
    vec3 normal = normalize(world_normal);
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = vec3(0.5f) * diff;

    // 高光
    vec3 view_dir = normalize(camera_pos - world_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), SPECULAR_EXPONENT);
    vec3 specular = SPECULAR_COLOR * spec;

    // 合成光照
    vec3 result = AMBIENT + diffuse + specular;

    final_color = vec4(result * object_color, 1.0f);
}
