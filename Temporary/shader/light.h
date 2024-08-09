/* ======================================================================== */
/* light.h                                                                  */
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

struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;
    float specular_exponent;
    vec3 specular_color;
    float ambient;
};

vec3 lighting(DirectionalLight light, vec3 v_world_normal, vec3 v_world_position, vec3 v_camera_position)
{
    vec3 light_dir = normalize(light.direction);

    vec3 normal = normalize(v_world_normal);
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = light.color * diff;

    vec3 view_dir = normalize(v_camera_position - v_world_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), light.specular_exponent);
    vec3 specular = light.specular_color * spec;

    return (vec3(light.ambient) + diffuse + specular);
}