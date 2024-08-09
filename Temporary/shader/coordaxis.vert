#version 450

float t = 0x7F7F;

vec3 vertices[4] = {
    {  t,    0.0f,  0.0f },
    { -t,    0.0f,  0.0f },
    {  0.0f, 0.0f,    t  },
    {  0.0f, 0.0f,   -t  },
};

// in
layout(set = 0, binding = 0) uniform Scene {
    vec4 camera_pos;
    mat4 projection;
    mat4 view;
} scene;

// out
layout(location = 0) out vec4 color;

void main()
{
    vec3 vertex;
    vertex = vertices[gl_VertexIndex];

    bool is_direction_x = abs(vertex.x) > 1;

    color = is_direction_x ? vec4(1.0f, 0.3f, 0.3f, 1.0f) :
                             vec4(0.3f, 1.0f, 0.3f, 1.0f);

    gl_Position = scene.projection * scene.view * vec4(vertex, 1.0f);
}