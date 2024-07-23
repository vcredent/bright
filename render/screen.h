/* ======================================================================== */
/* render_screen.h                                                          */
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
#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "drivers/render_device.h"
#include "platform/window.h"

class Screen {
public:
    Screen(RenderDevice *p_render_device);
   ~Screen();

    VkRenderPass get_render_pass() { return window->render_pass; }
    void *get_native_window() { return window->native_window_handle; }
    uint32_t get_image_buffer_count() { return window->image_buffer_count; }

    void initialize(Window *screen_window);
    VkCommandBuffer cmd_begin_window_render();
    void cmd_end_window_render(VkCommandBuffer cmd_buffer);

private:
    struct SwapchainResource {
        VkCommandBuffer cmd_buffer;
        VkImage image;
        VkImageView image_view;
        VkFramebuffer framebuffer;
    };

    struct _Window {
        VkSurfaceKHR vk_surface = VK_NULL_HANDLE;
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
        void *native_window_handle;
    };

    void _create_swap_chain();
    void _clean_up_swap_chain();
    void _update_swap_chain();

    RenderDevice *rd = VK_NULL_HANDLE;
    VkInstance vk_instance = VK_NULL_HANDLE;
    VkPhysicalDevice vk_physical_device = VK_NULL_HANDLE;
    VkDevice vk_device = VK_NULL_HANDLE;\
    uint32_t vk_graph_queue_family = 0;
    VkCommandPool vk_cmd_pool = VK_NULL_HANDLE;
    _Window *window = VK_NULL_HANDLE;
    VkQueue vk_graph_queue = VK_NULL_HANDLE;

    uint32_t acquire_next_index;
};

#endif /* _SCREEN_H_ */
