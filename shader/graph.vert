#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 v_normal;

layout(set = 0, binding = 0) uniform Scene {
    vec4 camera_pos;
    mat4 projection;
    mat4 view;
} scene;

layout(push_constant) uniform PushConst {
    mat4 model;
} push_const;

// out
layout(location = 0) out vec3 v_object_color;
layout(location = 1) out vec3 v_world_normal;
layout(location = 2) out vec3 v_world_position;

void main()
{
    gl_Position = scene.projection * scene.view * push_const.model * vec4(vertex, 1.0f);

    v_object_color = vec3(1.0f, 0.5f, 0.31f);
    v_world_normal = normalize(mat3(push_const.model) * v_normal);
    v_world_position = normalize(mat3(push_const.model) * vertex);
}