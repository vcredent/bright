#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

layout(set = 0, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
} scene;

layout(push_constant) uniform PushConst {
    mat4 model;
} push_const;

// out
layout(location = 0) out vec3 v_object_color;
layout(location = 1) out vec3 v_object_normal;

void main() {
    v_object_normal = normalize(scene.view * vec4(normal, 1.0f)).xyz;
    v_object_color = vec3(1.0f, 1.0f, 1.0f);
    gl_Position = scene.projection * scene.view * push_const.model * vec4(vertex, 1.0f);
}