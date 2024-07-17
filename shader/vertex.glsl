#version 450

// in
layout(location = 0) in vec3 position;
// out
layout(location = 0) out vec3 color;

void main()
{
    color = position;
    gl_Position = vec4(position, 1.0f);
}