/* ======================================================================== */
/* renderer_screen.cpp                                                      */
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
#include "renderer_screen.h"
#include <algorithm>

RendererScreen::RendererScreen(RenderDevice *p_render_device)
    : rd(p_render_device)
{
    vk_instance = rd->get_device_context()->get_instance();
    vk_physical_device = rd->get_device_context()->get_physical_device();
    vk_device = rd->get_device_context()->get_device();
    vk_graph_queue_family = rd->get_device_context()->get_graph_queue_family();
    vk_cmd_pool = rd->get_device_context()->get_cmd_pool();
    vk_graph_queue = rd->get_device_context()->get_graph_queue();
}

RendererScreen::~RendererScreen()
{
    vkDestroySemaphore(vk_device, window->image_available_semaphore, allocation_callbacks);
    vkDestroySemaphore(vk_device, window->render_finished_semaphore, allocation_callbacks);
    vkDestroySwapchainKHR(vk_device, window->swap_chain, allocation_callbacks);
    vkDestroyRenderPass(vk_device, window->render_pass, allocation_callbacks);
    _clean_up_swap_chain();
    vkDestroySurfaceKHR(vk_instance, window->vk_surface, allocation_callbacks);
    free(window);
}

void RendererScreen::initialize(Window *v_focused_window)
{
    VkResult U_ASSERT_ONLY err;

    /* imalloc display window struct and set surface */
    window = (_Window *) imalloc(sizeof(_Window));
    focused_window = v_focused_window;
    focused_window->create_vulkan_surface(vk_instance, allocation_callbacks, &window->vk_surface);

    VkSurfaceCapabilitiesKHR capabilities;
    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, window->vk_surface, &capabilities);
    assert(!err);

    /* pick surface format */
    uint32_t format_count = 0;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, window->vk_surface, &format_count, nullptr);
    assert(!err);

    VkSurfaceFormatKHR *surface_formats_khr = (VkSurfaceFormatKHR *) imalloc(sizeof(VkSurfaceFormatKHR) * format_count);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, window->vk_surface, &format_count, surface_formats_khr);
    assert(!err);

    VkSurfaceFormatKHR surface_format = pick_surface_format(surface_formats_khr, format_count);
    window->format = surface_format.format;
    window->color_space = surface_format.colorSpace;

    free(surface_formats_khr);

    /* image buffer count */
    uint32_t desired_buffer_count = 3;
    desired_buffer_count = std::clamp(desired_buffer_count, capabilities.minImageCount, capabilities.maxImageCount);
    window->image_buffer_count = desired_buffer_count;

    window->composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    window->present_mode = VK_PRESENT_MODE_FIFO_KHR;

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    err = vkCreateSemaphore(vk_device, &semaphore_create_info, allocation_callbacks, &window->image_available_semaphore);
    assert(!err);

    err = vkCreateSemaphore(vk_device, &semaphore_create_info, allocation_callbacks, &window->render_finished_semaphore);
    assert(!err);

    _create_swap_chain();
}

void RendererScreen::cmd_begin_screen_render(VkCommandBuffer *p_cmd_buffer)
{
    _update_swap_chain();
    vkAcquireNextImageKHR(vk_device, window->swap_chain, UINT64_MAX, window->image_available_semaphore, nullptr, &acquire_next_index);

    VkCommandBuffer cmd_buffer;
    cmd_buffer = window->swap_chain_resources[acquire_next_index].cmd_buffer;
    rd->cmd_buffer_begin(cmd_buffer, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

    VkClearValue clear_color = {
            0.10f, 0.10f, 0.10f, 1.0f
    };

    VkRect2D rect = {};
    rect.extent = { window->width, window->height };
    rd->cmd_begin_render_pass(cmd_buffer, window->render_pass, 1, &clear_color, window->swap_chain_resources[acquire_next_index].framebuffer, &rect);

    *p_cmd_buffer = cmd_buffer;
}

void RendererScreen::cmd_end_screen_render(VkCommandBuffer cmd_buffer)
{
    rd->cmd_end_render_pass(cmd_buffer);
    rd->cmd_buffer_end(cmd_buffer);

    VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    rd->cmd_buffer_submit(cmd_buffer, 1, &window->image_available_semaphore, 1, &window->render_finished_semaphore, &mask, vk_graph_queue, VK_NULL_HANDLE);
    rd->present(vk_graph_queue, window->swap_chain, acquire_next_index, window->render_finished_semaphore);
}

void RendererScreen::_create_swap_chain()
{
    VkResult U_ASSERT_ONLY err;
    VkSwapchainKHR old_swap_chain;

    old_swap_chain = window->swap_chain;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, window->vk_surface, &capabilities);
    window->width = capabilities.currentExtent.width;
    window->height = capabilities.currentExtent.height;

    if (!old_swap_chain) {
        // attachment
        VkAttachmentDescription color_attachment = {
                /* flags */ no_flag_bits,
                /* format */ window->format,
                /* samples */ VK_SAMPLE_COUNT_1_BIT,
                /* loadOp */VK_ATTACHMENT_LOAD_OP_CLEAR,
                /* storeOp */ VK_ATTACHMENT_STORE_OP_STORE,
                /* stencilLoadOp */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
                /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
                /* finalLayout */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
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

        // create render pass
        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &color_attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &subpass_dependency;

        err = vkCreateRenderPass(vk_device, &render_pass_create_info, allocation_callbacks, &window->render_pass);
        assert(!err);
    }

    /* create swap chain */
    VkSwapchainCreateInfoKHR swap_chain_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* surface */ window->vk_surface,
            /* minImageCount */ window->image_buffer_count,
            /* imageFormat */ window->format,
            /* imageColorSpace */ window->color_space,
            /* imageExtent */ capabilities.currentExtent,
            /* imageArrayLayers */ 1,
            /* imageUsage */ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            /* imageSharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 1,
            /* pQueueFamilyIndices */ &vk_graph_queue_family,
            /* preTransform */ capabilities.currentTransform,
            /* compositeAlpha */ window->composite_alpha,
            /* presentMode */ window->present_mode,
            /* clipped */ VK_TRUE,
            /* oldSwapchain */ old_swap_chain,
    };

    err = vkCreateSwapchainKHR(vk_device, &swap_chain_create_info, allocation_callbacks, &window->swap_chain);
    assert(!err);

    if (old_swap_chain)
        vkDestroySwapchainKHR(vk_device, old_swap_chain, allocation_callbacks);

    /* initialize swap chain resources */
    window->swap_chain_resources = (SwapchainResource *) imalloc(sizeof(SwapchainResource) * window->image_buffer_count);

    VkImage swap_chain_images[window->image_buffer_count];
    err = vkGetSwapchainImagesKHR(vk_device, window->swap_chain, &window->image_buffer_count, swap_chain_images);
    assert(!err);

    for (uint32_t i = 0; i < window->image_buffer_count; i++) {
        window->swap_chain_resources[i].image = swap_chain_images[i];

        VkCommandBufferAllocateInfo cmd_allocate_info = {
                /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                /* pNext */ nextptr,
                /* commandPool */ vk_cmd_pool,
                /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                /* commandBufferCount */ 1
        };

        vkAllocateCommandBuffers(vk_device, &cmd_allocate_info, &(window->swap_chain_resources[i].cmd_buffer));

        VkImageViewCreateInfo image_view_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                /* pNext */ nextptr,
                /* flags */ no_flag_bits,
                /* image */ window->swap_chain_resources[i].image,
                /* viewType */ VK_IMAGE_VIEW_TYPE_2D,
                /* format */ window->format,
                /* components */
                    {
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                    },
                /* subresourceRange */
                    {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                    },
        };

        err = vkCreateImageView(vk_device, &image_view_create_info, allocation_callbacks, &(window->swap_chain_resources[i].image_view));
        assert(!err);

        VkImageView framebuffer_attachments[] = { window->swap_chain_resources[i].image_view };

        VkFramebufferCreateInfo framebuffer_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                /* pNext */ nextptr,
                /* flags */ no_flag_bits,
                /* renderPass */ window->render_pass,
                /* attachmentCount */ ARRAY_SIZE(framebuffer_attachments),
                /* pAttachments */ framebuffer_attachments,
                /* width */ window->width,
                /* height */ window->height,
                /* layers */ 1,
        };

        err = vkCreateFramebuffer(vk_device, &framebuffer_create_info, allocation_callbacks, &(window->swap_chain_resources[i].framebuffer));
        assert(!err);
    }
}

void RendererScreen::_clean_up_swap_chain()
{
    for (uint32_t i = 0; i < window->image_buffer_count; i++) {
        vkFreeCommandBuffers(vk_device, vk_cmd_pool, 1, &window->swap_chain_resources[i].cmd_buffer);
        vkDestroyFramebuffer(vk_device, window->swap_chain_resources[i].framebuffer, allocation_callbacks);
        vkDestroyImageView(vk_device, window->swap_chain_resources[i].image_view, allocation_callbacks);
    }

    free(window->swap_chain_resources);
}

void RendererScreen::_update_swap_chain()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, window->vk_surface, &capabilities);
    VkExtent2D extent = capabilities.currentExtent;

    /* is update */
    if ((extent.width != window->width || extent.height != window->height) && (extent.width != 0 || extent.height != 0)) {
        vkDeviceWaitIdle(vk_device);
        _clean_up_swap_chain();
        _create_swap_chain();
    }
}
