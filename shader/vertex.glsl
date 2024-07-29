#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

layout(binding = 0) uniform Transform {
    mat4 m;
    mat4 v;
    mat4 p;
} transform;

// out
layout(location = 0) out vec3 out_posiiton;
layout(location = 2) out vec2 out_texcoord;
layout(location = 1) out vec3 out_normal;

void main() {

    vec4 world_position = transform.m * vec4(vertex, 1.0f);
    out_posiiton = world_position.xyz;
    out_texcoord = texcoord;
    out_normal = mat3(transpose(inverse(transform.m))) * normal;

    gl_Position = transform.p * transform.v * transform.m * vec4(vertex, 1.0f);
}