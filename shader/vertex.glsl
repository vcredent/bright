#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

layout(set = 0, binding = 0) uniform Transform {
    mat4 m;
    mat4 v;
    mat4 p;
} transform;

layout(push_constant) uniform PushConst {
    mat4 model;
} push_const;

// out
layout(location = 1) out vec3 v_normal;

void main() {
    v_normal = normalize(transform.v * vec4(normal, 1.0f)).xyz;
    gl_Position = transform.p * transform.v * push_const.model * vec4(vertex, 1.0f);
}