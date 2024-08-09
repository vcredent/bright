/* ======================================================================== */
/* rendering_graphics.cpp                                                   */
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
#include "rendering_graphics.h"

RenderingGraphics::RenderingGraphics(RenderDevice *v_rd, SceneRenderData *v_render_data)
    : rd(v_rd), render_data(v_render_data)
{
    /* do nothing... */
}

RenderingGraphics::~RenderingGraphics()
{
    rd->destroy_descriptor_set_layout(descriptor_set_layout);
    rd->free_descriptor_set(descriptor_set);
    rd->destroy_pipeline(pipeline);
}

void RenderingGraphics::initialize(VkRenderPass render_pass)
{
    VkVertexInputBindingDescription binds[] = {
            { 0, sizeof(RenderObject::Mesh), VK_VERTEX_INPUT_RATE_VERTEX  }
    };

    VkVertexInputAttributeDescription attributes[] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(RenderObject::Mesh, vertex) },
            { 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(RenderObject::Mesh, texcoord) },
            { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(RenderObject::Mesh, normal) },
    };

    VkDescriptorSetLayoutBinding descriptor_layout_binds[] = {
            SceneRenderData::GetPerspectiveDescriptorBindZero(),
            SceneRenderData::GetLightDescriptorBindOne(),
    };

    rd->create_descriptor_set_layout(ARRAY_SIZE(descriptor_layout_binds), descriptor_layout_binds, &descriptor_set_layout);
    rd->allocate_descriptor_set(descriptor_set_layout, &descriptor_set);
    render_data->set_descriptor_buffers(descriptor_set);

    VkPushConstantRange push_const_range = {
            /* stageFlags= */ VK_SHADER_STAGE_VERTEX_BIT,
            /* offset= */ 0,
            /* size= */ sizeof(mat4)
    };

    RenderDevice::ShaderInfo shader_info = {
            /* vertex= */ "graph",
            /* fragment= */ "graph",
            /* attribute_count= */ ARRAY_SIZE(attributes),
            /* attributes= */ attributes,
            /* bind_count= */ ARRAY_SIZE(binds),
            /* binds= */ binds,
            /* descriptor_count= */ 1,
            /* descriptor_layouts= */ &descriptor_set_layout,
            /* push_const_count= */ 1,
            /* p_push_const_range= */ &push_const_range,
    };

    RenderDevice::PipelineCreateInfo create_info = {
            /* render_pass= */ render_pass,
            /* polygon= */ VK_POLYGON_MODE_FILL,
            /* topology= */ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            /* cull_mode= */ VK_CULL_MODE_BACK_BIT,
            /* sample_count= */ rd->get_msaa_samples(),
    };

    pipeline = rd->create_graphics_pipeline(&create_info, &shader_info);
}

void RenderingGraphics::list_render_object(std::vector<RenderObject *> **p_objects)
{
    *p_objects = &render_objects;
}

void RenderingGraphics::push_render_object(RenderObject *object)
{
    render_objects.push_back(object);
}

void RenderingGraphics::cmd_draw_object_list(VkCommandBuffer cmd_buffer)
{
    rd->cmd_bind_pipeline(cmd_buffer, pipeline);
    rd->cmd_setval_viewport(cmd_buffer, render_data->get_scene_width(), render_data->get_scene_height());
    rd->cmd_bind_descriptor_set(cmd_buffer, pipeline, descriptor_set);

    for (auto &object: render_objects) {
        object->cmd_draw(cmd_buffer, pipeline);
    }
}