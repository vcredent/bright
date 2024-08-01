#version 450

// in
layout(location = 0) in vec4 v_color;

// out
layout(location = 0) out vec4 color;

void main()
{
    color = v_color;
}