#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inColor;

layout(binding = 0) uniform MVPMatrix {
    mat4 m;
    mat4 v;
    mat4 p;
} mmvp;

// out
layout(location = 0) out vec3 outColor;

void main() {
    gl_Position = mmvp.p * mmvp.v * mmvp.m * vec4(inPosition, 1.0f);
    outColor = inColor;
}