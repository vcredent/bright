/* ======================================================================== */
/* rendering_axis_line.cpp                                                  */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           COPILOT ENGINE                                 */
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
#include "rendering_axis_line.h"

RenderingAxisLine::RenderingAxisLine(RenderDevice *v_rd, SceneRenderData *v_render_data)
    : rd(v_rd), render_data(v_render_data)
{
    /* do nothing... */
}

RenderingAxisLine::~RenderingAxisLine()
{
    rd->destroy_pipeline(pipeline);
    rd->free_descriptor_set(descriptor_set);
    rd->destroy_descriptor_set_layout(descriptor_set_layout);
}

void RenderingAxisLine::initialize(VkRenderPass render_pass)
{
    VkDescriptorSetLayoutBinding binds[] = {
            SceneRenderData::GetDescriptorBindZero(),
    };

    rd->create_descriptor_set_layout(ARRAY_SIZE(binds), binds, &descriptor_set_layout);
    rd->allocate_descriptor_set(descriptor_set_layout, &descriptor_set);
    rd->update_descriptor_buffer(render_data->get_uniform_buffer(), 0, descriptor_set);

    RenderDevice::ShaderInfo shader_info = {
            "axis_line_vertex",
            "axis_line_fragment",
            0, NULL,
            0, NULL,
            1, &descriptor_set_layout,
    };

    RenderDevice::PipelineCreateInfo create_info = {
            /* render_pass= */ render_pass,
            /* polygon= */ VK_POLYGON_MODE_FILL,
            /* topology= */ VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
            /* samples= */ rd->get_msaa_samples(),
            /* line_width= */ 2.0f,
    };

    pipeline = rd->create_graphics_pipeline(&create_info, &shader_info);
}

void RenderingAxisLine::cmd_draw_line(VkCommandBuffer cmd_buffer)
{
    rd->cmd_bind_pipeline(cmd_buffer, pipeline);
    rd->cmd_setval_viewport(cmd_buffer, render_data->get_scene_width(), render_data->get_scene_height());
    rd->cmd_bind_descriptor_set(cmd_buffer, pipeline, descriptor_set);
    vkCmdSetLineWidth(cmd_buffer, 2.0f);
    rd->cmd_draw(cmd_buffer, 4);
}