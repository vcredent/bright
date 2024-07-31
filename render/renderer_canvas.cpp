/* ======================================================================== */
/* renderer_canvas.cpp                                                      */
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
#include "renderer_canvas.h"
#include <array>

RendererCanvas::RendererCanvas(RenderDevice *p_device)
    : rd(p_device)
{
    rdc = rd->get_device_context();
}

RendererCanvas::~RendererCanvas()
{
    _clean_up_canvas_texture();
    rd->destroy_sampler(sampler);
    rd->destroy_render_pass(render_pass);
    rd->free_cmd_buffer(canvas_cmd_buffer);
}

void RendererCanvas::initialize()
{
    graph_queue = rd->get_device_context()->get_graph_queue();

    std::vector<VkFormat> desred_depth_formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    depth_format = rdc->find_supported_format(desred_depth_formats ,VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    /* ************************ */
    /*        RenderPass        */
    /* ************************ */
    VkAttachmentDescription attachments[] = {
            {
                /* flags */ no_flag_bits,
                /* format */ rd->get_surface_format(),
                /* samples */ VK_SAMPLE_COUNT_1_BIT,
                /* loadOp */ VK_ATTACHMENT_LOAD_OP_CLEAR,
                /* storeOp */ VK_ATTACHMENT_STORE_OP_STORE,
                /* stencilLoadOp */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
                /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
                /* finalLayout */ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            },
            {
                /* flags */ no_flag_bits,
                /* format */ depth_format,
                /* samples */ VK_SAMPLE_COUNT_1_BIT,
                /* loadOp */VK_ATTACHMENT_LOAD_OP_CLEAR,
                /* storeOp */ VK_ATTACHMENT_STORE_OP_STORE,
                /* stencilLoadOp */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
                /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
                    /* finalLayout */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            }
    };

    VkAttachmentReference color_reference = {
            /* attachment= */ 0,
            /* layout= */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depth_reference = {
            /* attachment= */ 1,
            /* layout= */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pDepthStencilAttachment = &depth_reference;

    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask =  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    rd->create_render_pass(ARRAY_SIZE(attachments), attachments, 1, &subpass, 1, &subpass_dependency, &render_pass);
    rd->create_sampler(&sampler);
    rd->allocate_cmd_buffer(&canvas_cmd_buffer);

    _create_canvas_texture(width, height, true);
}

void RendererCanvas::cmd_begin_canvas_render(VkCommandBuffer *p_cmd_buffer)
{
    if (texture->width != width || texture->height != height) {
        _clean_up_canvas_texture();
        _create_canvas_texture(width, height, false);
    }

    rd->cmd_buffer_begin(canvas_cmd_buffer, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

    std::array<VkClearValue, 2> clear_values = {};
    clear_values[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
    clear_values[1].depthStencil = { 1.0f, 0 };

    VkRect2D rect = {};
    rect.offset = { 0, 0 };
    rect.extent = { texture->width, texture->height };
    rd->cmd_begin_render_pass(canvas_cmd_buffer, render_pass, std::size(clear_values), std::data(clear_values), framebuffer, &rect);

    *p_cmd_buffer = canvas_cmd_buffer;
}

void RendererCanvas::set_canvas_extent(uint32_t v_width, uint32_t v_height)
{
    width  = v_width;
    height = v_height;
}

RenderDevice::Texture2D *RendererCanvas::cmd_end_canvas_render()
{
    rd->cmd_end_render_pass(canvas_cmd_buffer);
    rd->cmd_buffer_end(canvas_cmd_buffer);

    // submit
    rd->cmd_buffer_submit(canvas_cmd_buffer,
                          0, nullptr,
                          0, nullptr,
                          nullptr,
                          graph_queue,
                          nullptr);

    return texture;
}

void RendererCanvas::_create_canvas_texture(uint32_t width, uint32_t height)
{
    depth = rd->create_texture(width, height, sampler, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    texture = rd->create_texture(width, height, sampler, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    VkCommandBuffer cmd_buffer;
    rd->cmd_buffer_one_time_begin(&cmd_buffer);
    RenderDevice::PipelineMemoryBarrier barrier = {};
    barrier.image_memory_barrier.texture = texture;
    barrier.image_memory_barrier.src_access_mask = VK_ACCESS_NONE;
    barrier.image_memory_barrier.dst_access_mask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_HOST_READ_BIT;
    barrier.image_memory_barrier.old_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.image_memory_barrier.new_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    rd->cmd_pipeline_barrier(cmd_buffer, &barrier);
    rd->cmd_buffer_one_time_end(cmd_buffer);

    VkImageView attachments[] = {
            texture->image_view,
            depth->image_view,
    };

    rd->create_framebuffer(texture->width, texture->height, ARRAY_SIZE(attachments), attachments, render_pass, &framebuffer);
}

void RendererCanvas::_clean_up_canvas_texture()
{
    rd->destroy_texture(depth);
    rd->destroy_texture(texture);
    texture = VK_NULL_HANDLE;
    rd->destroy_framebuffer(framebuffer);
    framebuffer = VK_NULL_HANDLE;
}