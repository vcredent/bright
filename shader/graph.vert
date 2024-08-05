#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

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
layout(location = 3) out vec3 v_camera_pos;

void main()
{
    vec4 world_position = push_const.model * vec4(vertex, 1.0f);
    gl_Position = scene.projection * scene.view * world_position;

    v_object_color = vec3(1.0f, 1.0f, 1.0f);
    v_world_normal = mat3(transpose(inverse(push_const.model))) * normal;
    v_world_position = vec3(world_position);
    v_camera_pos = scene.camera_pos.xyz;
}