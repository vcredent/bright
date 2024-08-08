/* ======================================================================== */
/* rendering_sky_sphere.h                                                   */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#ifndef _RENDERING_SKY_SPHERE_H_
#define _RENDERING_SKY_SPHERE_H_

#include "drivers/render_device.h"
#include "scene_render_data.h"
#include <bright/properties.h>

class RenderingSkySphere : public NodeProperties {
public:
    U_MEMNEW_ONLY RenderingSkySphere(RenderDevice* v_rd, SceneRenderData* v_render_data);
   ~RenderingSkySphere();

    void initialize(VkRenderPass v_render_pass);
    void cmd_draw_sky_sphere(VkCommandBuffer cmd_buffer);

private:
    struct Vertex {
        vec3 position;
        vec2 texcoord;
        vec3 normal;
    };

    struct PushConst {
        mat4 transform;
        float exposure;
        float gamma;
    };

    RenderDevice* rd;
    SceneRenderData* render_data;

    RenderDevice::Pipeline* pipeline;
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet descriptor_set;
    RenderDevice::Texture2D* hdr;
    VkSampler hdr_sampler;
    RenderDevice::Buffer* vertex_buffer;
    RenderDevice::Buffer* index_buffer;
    uint32_t index_count;

    float exposure = 0.5f;
    float gamma = 2.02f;
};

#endif /* _RENDERING_SKY_SPHERE_H_ */
