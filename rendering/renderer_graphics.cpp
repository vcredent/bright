/* ======================================================================== */
/* renderer_graphics.cpp                                                    */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#include "renderer_graphics.h"

RendererGraphics::RendererGraphics(RenderDevice *v_rd)
    : rd(v_rd)
{
    /* do nothing... */
}

RendererGraphics::~RendererGraphics()
{
    rd->destroy_descriptor_set_layout(descriptor_set_layout);
    rd->free_descriptor_set(descriptor_set);
    rd->destroy_buffer(transform_buffer);
    rd->destroy_pipeline(pipeline);
}

void RendererGraphics::initialize(VkRenderPass render_pass)
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
            { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE },
    };

    rd->create_descriptor_set_layout(ARRAY_SIZE(descriptor_layout_binds), descriptor_layout_binds, &descriptor_set_layout);
    rd->allocate_descriptor_set(descriptor_set_layout, &descriptor_set);

    transform_buffer = rd->create_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Transform));
    rd->write_descriptor_set_buffer(transform_buffer, descriptor_set);

    VkPushConstantRange push_const_range = {
            /* stageFlags= */ VK_SHADER_STAGE_VERTEX_BIT,
            /* offset= */ 0,
            /* size= */ sizeof(Mat4)
    };

    RenderDevice::ShaderInfo shader_info = {
            /* vertex= */ "../shader/vertex.glsl.spv",
            /* fragment= */ "../shader/fragment.glsl.spv",
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
            /* sample_count= */ rd->get_msaa_samples(),
    };

    pipeline = rd->create_graphics_pipeline(&create_info, &shader_info);
}

void RendererGraphics::push_render_object(RenderObject *object)
{
    object->initialize(rd);
    render_objects.push_back(object);
}

void RendererGraphics::cmd_begin_graphics_render(VkCommandBuffer cmd_buffer)
{
    rd->cmd_bind_pipeline(cmd_buffer, pipeline);
}

void RendererGraphics::cmd_end_graphics_render(VkCommandBuffer cmd_buffer)
{
    /* do nothing... */
}

void RendererGraphics::cmd_setval_viewport(VkCommandBuffer cmd_buffer, uint32_t w, uint32_t h)
{
    rd->cmd_setval_viewport(cmd_buffer, w, h);
}

void RendererGraphics::cmd_setval_view_matrix(VkCommandBuffer U_MAYBE_UNUSED cmd_buffer, Mat4 view)
{
    transform.v = view;
    rd->write_buffer(transform_buffer, 0, sizeof(Transform), &transform);
}

void RendererGraphics::cmd_setval_projection_matrix(VkCommandBuffer U_MAYBE_UNUSED cmd_buffer, Mat4 projection)
{
    transform.p = projection;
    rd->write_buffer(transform_buffer, 0, sizeof(Transform), &transform);
}

void RendererGraphics::cmd_setval_model_matrix(VkCommandBuffer U_MAYBE_UNUSED cmd_buffer, Mat4 model)
{
    transform.m = model;
    rd->write_buffer(transform_buffer, 0, sizeof(Transform), &transform);
}

void RendererGraphics::cmd_draw_list(VkCommandBuffer cmd_buffer)
{
    rd->cmd_bind_descriptor_set(cmd_buffer, pipeline, descriptor_set);
    for (auto &object: render_objects) {
        object->update();
        Mat4 value = object->get_model_matrix();
        rd->cmd_push_const(cmd_buffer, pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Mat4), &value);
        object->cmd_bind_vertex_buffer(cmd_buffer);
        object->cmd_bind_index_buffer(cmd_buffer);
        object->cmd_draw(cmd_buffer);
    }
}