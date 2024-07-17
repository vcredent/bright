#version 450

layout(location = 0) in vec3 position;
// out
layout(location = 0) out vec4 final_color;

void main()
{
    final_color = vec4(position, 1.0f);
}