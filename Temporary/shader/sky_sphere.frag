/* ======================================================================== */
/* sky_sphere.frag                                                          */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                            BRIGHT ENGINE                                 */
/* ======================================================================== */
/*                                                                          */
/* Copyright (C) 2022 Vcredent All rights reserved.                         */
/*                                                                          */
/* Licensed under the Apache License, Version 2.0 (the "License");          */
/* you may not use this file except in compliance with the License.         */
/*                                                                          */
/* You may obtain a copy of the License at                                  */
/*     http://www.apache.org/licenses/LICENSE-2.0                           */
/*                                                                          */
/* Unless required by applicable law or agreed to in writing, software      */
/* distributed under the License is distributed on an "AS IS" BASIS,        */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#version 450
#extension GL_GOOGLE_include_directive : enable

#include "light.h"

layout(location = 0) in vec2 texcoord;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec3 world_position;
layout(location = 3) in vec3 camera_position;
layout(location = 4) in float exposure;
layout(location = 5) in float gamma;

layout(set = 0, binding = 1) uniform DescriptorSetBlock {
    DirectionalLight light;
};

layout(set = 0, binding = 2) uniform sampler2D hdr;

// outf
layout(location = 0) out vec4 final_color;

void main()
{
    // vec3 light = lighting(light, world_normal, world_position, camera_position);
    // vec4 tex = texture(hdr, texcoord);
    // final_color = vec4(light * tex.rgb, tex.a);

    vec3 hdr_color = texture(hdr, texcoord).rgb;
    vec3 color = hdr_color * exposure;
    vec3 gamma_corrected = pow(color, vec3(1.0f / gamma));

    final_color = vec4(gamma_corrected, 1.0f);
}
