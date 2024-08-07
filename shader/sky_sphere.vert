/* ======================================================================== */
/* sky_sphere.vert                                                          */
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

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

layout(set = 0, binding = 0) uniform Scene {
    vec4 camera_pos;
    mat4 projection;
    mat4 view;
} scene;

layout(push_constant) uniform PushConst {
    mat4 model;
    float exposure;
    float gamma;
} push_const;

// out
layout(location = 0) out vec2 v_texcoord;
layout(location = 1) out vec3 v_world_normal;
layout(location = 2) out vec3 v_world_position;
layout(location = 3) out vec3 v_camera_position;
layout(location = 4) out float v_exposure;
layout(location = 5) out float v_gamma;

void main()
{
    vec4 world_position = push_const.model * vec4(vertex, 1.0f);
    gl_Position = scene.projection * scene.view * world_position;
    v_texcoord = texcoord;
    v_world_normal = mat3(transpose(inverse(push_const.model))) * normal;
    v_world_position = vec3(world_position);
    v_camera_position = scene.camera_pos.xyz;
    v_exposure = push_const.exposure;
    v_gamma = push_const.gamma;
}