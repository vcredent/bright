/* ======================================================================== */
/* rendering_context_driver_vulkan.h                                        */
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
#ifndef _RENDERING_CONTEXT_DRIVER_VULKAN_H
#define _RENDERING_CONTEXT_DRIVER_VULKAN_H

#ifdef VOLK_LOADER
#  include <volk/volk.h>
#else
#  include <vulkan/vulkan.h>
#endif

#include <vma/vk_mem_alloc.h>

// Render context driver of vulkan
#include <engine/error.h>
#include <engine/typealias.h>

#define no_flag_bits         0
#define nextptr              nullptr
#define allocation_callbacks nullptr

class RenderingContextDriverVulkan {
public:
    RenderingContextDriverVulkan();
    ~RenderingContextDriverVulkan();

    Error initialize();
    VkInstance get_instance() { return instance; }
    VkPhysicalDevice get_physical_device() { return physical_device; }
    const char *get_device_name() { return physical_device_properties.deviceName; }
    VkDevice get_device() { return device; }
    VmaAllocator get_allocator() { return allocator; }

protected:
    struct Window {
        VkSurfaceKHR surface;
        VkSurfaceCapabilitiesKHR capabilities;
    };

    void _initialize_window(VkSurfaceKHR surface);

private:
    void _clean_up_window();

    void _create_device();
    void _create_command_pool();
    void _create_vma_allocator();
    void _create_swap_chain();

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
