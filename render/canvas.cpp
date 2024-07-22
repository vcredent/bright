/* ======================================================================== */
/* render_canvas.cpp                                                        */
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
#include "canvas.h"

Canvas::Canvas(RenderDevice *p_device)
    : rd(p_device)
{
    /* do nothing... */
}

Canvas::~Canvas()
{
    _clean_up_canvas_texture();
    rd->destroy_sampler(sampler);
    rd->destroy_render_pass(render_pass);
    rd->free_command_buffer(canvas_command_buffer);
}

void Canvas::initialize()
{
    graph_queue = rd->get_device_context()->get_graph_queue();

    /* ************************ */
    /*        RenderPass        */
    /* ************************ */
    VkAttachmentDescription color_attachment = {
            /* flags */ no_flag_bits,
            /* format */ rd->get_surface_format(),
            /* samples */ VK_SAMPLE_COUNT_1_BIT,
            /* loadOp */VK_ATTACHMENT_LOAD_OP_CLEAR,
            /* storeOp */ VK_ATTACHMENT_STORE_OP_STORE,
            /* stencilLoadOp */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
            /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
            /* finalLayout */ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkAttachmentReference color_attachment_reference = {};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    rd->create_render_pass(1, &color_attachment, 1, &subpass, 1, &subpass_dependency, &render_pass);
    rd->create_sampler(&sampler);
    rd->allocate_command_buffer(&canvas_command_buffer);

    _create_canvas_texture(32, 32);
}

void Canvas::command_begin_canvas_render(VkCommandBuffer *p_command_buffer, uint32_t width, uint32_t height)
{
    if (texture->width != width || texture->height != height) {
        _clean_up_canvas_texture();
        _create_canvas_texture(width, height);
    }

    rd->command_buffer_begin(canvas_command_buffer, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

    VkRect2D rect = {};
    rect.offset = { 0, 0 };
    rect.extent = { texture->width, texture->height };
    rd->command_begin_render_pass(canvas_command_buffer, render_pass, framebuffer, &rect);

    *p_command_buffer = canvas_command_buffer;
}

RenderDevice::Texture2D *Canvas::command_end_canvas_render()
{
    rd->command_end_render_pass(canvas_command_buffer);
    rd->command_buffer_end(canvas_command_buffer);

    // submit
    rd->command_buffer_submit(canvas_command_buffer,
                              0, nullptr,
                              0, nullptr,
                              nullptr,
                              graph_queue,
                              nullptr);

    return texture;
}

void Canvas::_create_canvas_texture(uint32_t width, uint32_t height)
{
    texture = rd->create_texture(width, height, sampler, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    rd->transition_image_layout(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    rd->create_framebuffer(texture->width, texture->height, 1, &texture->image_view, render_pass, &framebuffer);
}

void Canvas::_clean_up_canvas_texture()
{
    rd->destroy_texture(texture);
    texture = VK_NULL_HANDLE;
    rd->destroy_framebuffer(framebuffer);
    framebuffer = VK_NULL_HANDLE;
}