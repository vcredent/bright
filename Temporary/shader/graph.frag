#version 450
#extension GL_GOOGLE_include_directive : enable

layout(location = 0) in vec3 object_color;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec3 world_position;
layout(location = 3) in vec3 camera_position;

#include "light.h"

layout(set = 0, binding = 1) uniform DescriptorSetBlock {
    DirectionalLight light;
};

// out
layout(location = 0) out vec4 final_color;

void main()
{
    vec3 light = lighting(light, world_normal, world_position, camera_position);
    final_color = vec4(light * object_color, 1.0f);
}
