#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outPosition;

void main() {
    gl_Position = vec4(inPosition, 1.0f);
    outColor = inColor;
    outPosition = inPosition;
}