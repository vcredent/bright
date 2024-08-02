/* ======================================================================== */
/* renderer_axis_line.cpp                                                   */
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
#include "renderer_axis_line.h"

RendererAxisLine::RendererAxisLine(RenderDevice *v_rd)
    : rd(v_rd)
{
    /* do nothing... */
}

RendererAxisLine::~RendererAxisLine()
{
    rd->destroy_pipeline(pipeline);
    rd->destroy_buffer(uniform);
    rd->free_descriptor_set(descriptor_set);
    rd->destroy_descriptor_set_layout(descriptor_set_layout);
}

void RendererAxisLine::initialize(VkRenderPass render_pass)
{
    VkDescriptorSetLayoutBinding binds[] = {
            { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE },
    };

    rd->create_descriptor_set_layout(ARRAY_SIZE(binds), binds, &descriptor_set_layout);
    rd->allocate_descriptor_set(descriptor_set_layout, &descriptor_set);
    uniform = rd->create_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Matrix));
    rd->write_descriptor_set_buffer(uniform, descriptor_set);

    RenderDevice::ShaderInfo shader_info = {
            "../shader/axis_line_vertex.glsl.spv",
            "../shader/axis_line_fragment.glsl.spv",
            0, NULL,
            0, NULL,
            1, &descriptor_set_layout,
    };

    RenderDevice::PipelineCreateInfo create_info = {
            /* render_pass= */ render_pass,
            /* polygon= */ VK_POLYGON_MODE_FILL,
            /* topology= */ VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
            /* samples= */ rd->get_msaa_samples(),
    };

    pipeline = rd->create_graphics_pipeline(&create_info, &shader_info);
}

void RendererAxisLine::cmd_setval_viewport(VkCommandBuffer cmd_buffer, uint32_t w, uint32_t h)
{
    rd->cmd_setval_viewport(cmd_buffer, w, h);
}

void RendererAxisLine::cmd_draw_line(VkCommandBuffer cmd_buffer, Mat4 projection, Mat4 view)
{
    matrix.projection = projection;
    matrix.view = view;
    rd->cmd_bind_pipeline(cmd_buffer, pipeline);
    rd->cmd_bind_descriptor_set(cmd_buffer, pipeline, descriptor_set);
    rd->write_buffer(uniform, 0, sizeof(Matrix), &matrix);
    rd->cmd_draw(cmd_buffer, 4);
}