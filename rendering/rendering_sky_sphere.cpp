/* ======================================================================== */
/* rendering_sky_sphere.cpp                                                 */
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
#include "rendering_sky_sphere.h"
#include <stb/stb_image.h>
#include "modules/obj.h"

RenderingSkySphere::RenderingSkySphere(RenderDevice* v_rd, SceneRenderData* v_render_data)\
    : rd(v_rd), render_data(v_render_data)
{
    /* do nothing... */
}

RenderingSkySphere::~RenderingSkySphere()
{
    rd->destroy_buffer(index_buffer);
    rd->destroy_buffer(vertex_buffer);
    rd->free_descriptor_set(descriptor_set);
    rd->destroy_descriptor_set_layout(descriptor_set_layout);
    rd->destroy_pipeline(pipeline);
}

void RenderingSkySphere::initialize(VkRenderPass v_render_pass)
{
    ObjLoader *loader = ObjLoader::load(_CURDIR("resource/obj/sphere.obj"));
    std::vector<ObjLoader::Vertex> vertices = loader->get_vertices();
    std::vector<uint32_t> indices = loader->get_indices();

    size_t vertices_size = std::size(vertices) * sizeof(ObjLoader::Vertex);
    vertex_buffer = rd->create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertices_size);
    rd->write_buffer(vertex_buffer, 0, vertices_size, std::data(vertices));

    index_count = std::size(indices);
    size_t indices_size = index_count * sizeof(uint32_t);
    index_buffer = rd->create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices_size);
    rd->write_buffer(index_buffer, 0, indices_size, std::data(indices));

    ObjLoader::destroy(loader);

    VkVertexInputBindingDescription binds[] = {
        { 0, sizeof(ObjLoader::Vertex), VK_VERTEX_INPUT_RATE_VERTEX  }
    };

    VkVertexInputAttributeDescription attributes[] = {
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ObjLoader::Vertex, position) },
        { 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ObjLoader::Vertex, texcoord) },
        { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ObjLoader::Vertex, normal) },
    };

    VkDescriptorSetLayoutBinding desciprotr_layout_binds[] = {
        SceneRenderData::GetPerspectiveDescriptorBindZero(),
        SceneRenderData::GetLightDescriptorBindOne(),
    };

    rd->create_descriptor_set_layout(ARRAY_SIZE(desciprotr_layout_binds), desciprotr_layout_binds, &descriptor_set_layout);
    rd->allocate_descriptor_set(descriptor_set_layout, &descriptor_set);
    render_data->set_descriptor_buffers(descriptor_set);

    VkPushConstantRange range = {
        /* stageFlags= */ VK_SHADER_STAGE_VERTEX_BIT,
        /* offset= */ 0,
        /* size= */ sizeof(Mat4)
    };

    RenderDevice::ShaderInfo shader_info = {
        /* vertex= */ "sky_sphere",
        /* fragment= */ "sky_sphere",
        /* attribute_count= */ ARRAY_SIZE(attributes),
        /* attributes= */ attributes,
        /* bind_count= */ ARRAY_SIZE(binds),
        /* binds= */ binds,
        /* descriptor_set_layout_count= */ 1,
        /* p_descriptor_set_layouts= */ &descriptor_set_layout,
        /* push_const_count= */ 1,
        /* p_push_const_range= */ &range,
    };

    RenderDevice::PipelineCreateInfo create_info = {
        /* render_pass= */ v_render_pass,
        /* polygon= */ VK_POLYGON_MODE_FILL,
        /* topology= */ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        /* samples= */ rd->get_msaa_samples(),
    };

    pipeline = rd->create_graphics_pipeline(&create_info, &shader_info);
}

void RenderingSkySphere::cmd_draw_sky_sphere(VkCommandBuffer cmd_buffer)
{
    rd->cmd_bind_pipeline(cmd_buffer, pipeline);
    rd->cmd_setval_viewport(cmd_buffer, render_data->get_scene_width(), render_data->get_scene_height());
    rd->cmd_bind_descriptor_set(cmd_buffer, pipeline, descriptor_set);

    Mat4 mat(1.0f);

    Mat4 T(1.0f);
    T = glm::translate(T, Vec3(0.0f));

    Mat4 R(1.0f);
    R = glm::rotate(R, glm::radians(0.0f), Vec3(1.0f, 0.0f, 0.0f));
    R = glm::rotate(R, glm::radians(0.0f), Vec3(0.0f, 1.0f, 0.0f));
    R = glm::rotate(R, glm::radians(0.0f), Vec3(0.0f, 0.0f, 1.0f));

    Mat4 S(1.0f);
    S = glm::scale(S, Vec3(10.0f));

    mat = T * R * S;
    rd->cmd_push_const(cmd_buffer, pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Mat4), glm::value_ptr(mat));

    rd->cmd_bind_vertex_buffer(cmd_buffer, vertex_buffer);
    rd->cmd_bind_index_buffer(cmd_buffer, VK_INDEX_TYPE_UINT32, index_buffer);
    rd->cmd_draw_indexed(cmd_buffer, index_count);
}


