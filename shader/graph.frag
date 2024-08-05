#version 450

layout(location = 0) in vec3 object_color;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec3 world_position;

// out
layout(location = 0) out vec4 final_color;

const vec3 LIGHT_DIRECTION = normalize(vec3(-1.0f, -1.0f, -1.0f)); // 平行光
const vec3 LIGHT_COLOR = vec3(1.0f, 1.0f, 1.0f); // 光源颜色
const float LIGHT_INTSENTIY = 1.0f; // 光源强度
const float SPECULAR_EXPONENT = 32.0f; // 高光指数（控制镜面高光的硬度）
const float AMBIENT = 0.2f; // 环境光

void main()
{
    vec3 light_dir = normalize(LIGHT_DIRECTION - world_position);
    float diffuse = max(dot(light_dir, world_normal), 0.0f);

    vec3 _reflect = normalize(reflect(light_dir, world_normal));
    vec3 view_dir = normalize(world_position - _reflect);
    vec3 specular = LIGHT_COLOR * pow(max(dot(view_dir, _reflect), 0.0f), 100.0f) * SPECULAR_EXPONENT;

    vec3 result = specular + diffuse + (AMBIENT * LIGHT_COLOR);

    final_color = vec4(result * object_color, 1.0f);
}
