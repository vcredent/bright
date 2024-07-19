/* ======================================================================== */
/* render_device_context.h                                                  */
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
#ifndef _RENDERING_CONTEXT_DRIVER_VULKAN_H
#define _RENDERING_CONTEXT_DRIVER_VULKAN_H

#ifdef VOLK_LOADER
#  include <volk/volk.h>
#else
#  include <vulkan/vulkan.h>
#endif

#include <vma/vk_mem_alloc.h>

#include <copilot/error.h>
#include <copilot/typealias.h>
#include <time.h>

#define no_flag_bits         0
#define nextptr              nullptr
#define allocation_callbacks nullptr

#include "vulkan_utils.h"

// Render context driver of vulkan
class RenderDeviceContext {
public:
    RenderDeviceContext();
    ~RenderDeviceContext();

    Error initialize();
    VkInstance get_instance() { return instance; }
    VkPhysicalDevice get_physical_device() { return physical_device; }
    const char *get_device_name() { return physical_device_properties.deviceName; }
    VkDevice get_device() { return device; }
    VmaAllocator get_allocator() { return allocator; }
    uint32_t get_width() { return window->width; }
    uint32_t get_height() { return window->height; }
    VkRenderPass get_render_pass() { return window->render_pass; }
    void get_window_semaphore(VkSemaphore *p_available_semaphore, VkSemaphore *p_finished_semaphore);
    void get_graph_queue(VkQueue *p_queue) { *p_queue = graph_queue; };
    void get_swap_chain(VkSwapchainKHR *p_swap_chain) { *p_swap_chain = window->swap_chain; }

    struct AcquiredNext {
        VkSwapchainKHR swap_chain;
        VkSemaphore wait_semaphore;
        VkRenderPass render_pass;
        VkCommandBuffer command_buffer;
        VkFramebuffer framebuffer;
        uint32_t index;
        uint32_t width;
        uint32_t height;
    };

    void acquire_next_image(AcquiredNext *p_acquire_next);
    void allocate_command_buffer(VkCommandBufferLevel level, VkCommandBuffer *p_command_buffer);
    void free_command_buffer(VkCommandBuffer command_buffer);

    void update_window();

protected:
    struct SwapchainResource {
        VkCommandBuffer command_buffer;
        VkImage image;
        VkImageView image_view;
        VkFramebuffer framebuffer;
    };

    struct Window {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkFormat format;
        VkColorSpaceKHR color_space;
        uint32_t image_buffer_count;
        VkCompositeAlphaFlagBitsKHR composite_alpha;
        VkPresentModeKHR present_mode;
        VkRenderPass render_pass = VK_NULL_HANDLE;
        VkSwapchainKHR swap_chain = VK_NULL_HANDLE;
        SwapchainResource *swap_chain_resources;
        VkSemaphore image_available_semaphore;
        VkSemaphore render_finished_semaphore;
        uint32_t width;
        uint32_t height;
    };

    void _initialize_window(VkSurfaceKHR surface);

private:
    void _clean_up_window();

    void _create_device();
    void _initialize_window_semaphore();
    void _create_command_pool();
    void _create_vma_allocator();
    void _create_swap_chain();
    void _clean_up_swap_chain();
    void _update_swap_chain();

    VkInstance instance = VK_NULL_HANDLE;
    Window *window = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceFeatures physical_device_features;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t graph_queue_family;
    VkQueue graph_queue = VK_NULL_HANDLE;
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VmaAllocator allocator = VK_NULL_HANDLE;
};

#endif /* _RENDERING_CONTEXT_DRIVER_VULKAN_H */
