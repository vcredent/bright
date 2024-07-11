/* ======================================================================== */
/* vulkan_context.h                                                         */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           PORTABLE ENGINE                                */
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
#include "hardware.h"

#ifdef VOLK_LOADER
#  include <volk/volk.h>
# else
#  include <vulkan/vulkan.h>
#endif

#include <vma/vk_mem_alloc.h>

class VkContext : public HardwareDevice {
public:
    explicit VkContext();
    ~VkContext() override;

public:
    VkInstance get_instance() { return inst; }
    VkDevice get_device() { return device; }
    const char *get_device_name() { return gpu_properties.deviceName; }
    VmaAllocator get_allocator() { return allocator; }

protected:
    void _window_create(VkSurfaceKHR surface); /* initialize */
    void _clean_up_context(); /* destroy */

private:
    void _create_physical_device(VkSurfaceKHR surface);
    void _initialize_queues(VkPhysicalDevice gpu, VkSurfaceKHR surface);
    void _create_device(VkDevice *p_device);
    void _initialize_vma(VkInstance inst, VkPhysicalDevice gpu, VkDevice device, VmaAllocator *p_allocator);

    typedef struct SwapchainImageResource {
        VkImage image;
        VkImageView image_view;
        VkFramebuffer framebuffer;
        VkCommandBuffer command_buffer;
    } SwapchainImageResource;

    typedef struct Window {
        VkSurfaceKHR surface;
        VkSurfaceCapabilitiesKHR capabilities;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkFormat format;
        VkColorSpaceKHR colorspace;
        uint32_t image_buffer_count;
        VkSurfaceTransformFlagBitsKHR transform;
        uint32_t width;
        uint32_t height;
        VkCompositeAlphaFlagBitsKHR composite_alpha;
        VkPresentModeKHR present_mode;
        SwapchainImageResource *swap_chain_resources = VK_NULL_HANDLE;
        VkCommandPool command_pool;
        VkRenderPass render_pass;
    } Window;

    void _initialize_window(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
    void _create_swap_chain(VkDevice device, Window *window);
    void _clean_up_swap_chain(VkDevice device, Window *window);
    void _update_swap_chain(VkDevice device, Window *window);

private:
    VkInstance inst;
    VkPhysicalDevice gpu;
    VkPhysicalDeviceProperties gpu_properties;
    VkPhysicalDeviceFeatures gpu_features;
    VkDevice device;
    VmaAllocator allocator = VK_NULL_HANDLE;
    uint32_t graph_queue_family;
    VkQueue graph_command_queue;
    Window *window = VK_NULL_HANDLE;
};
