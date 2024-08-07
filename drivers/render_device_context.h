/* ======================================================================== */
/* render_device_context.h                                                  */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                            BRIGHT ENGINE                                 */
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
#ifndef _RENDERING_CONTEXT_DRIVER_VULKAN_H
#define _RENDERING_CONTEXT_DRIVER_VULKAN_H

#ifdef VOLK_LOADER
#  include <volk/volk.h>
#else
#  include <vulkan/vulkan.h>
#endif

#include <vma/vk_mem_alloc.h>

#include <bright/error.h>
#include <bright/typedefs.h>
#include <time.h>
#include <vector>

#define no_flag_bits         0
#define nextptr              nullptr
#define allocation_callbacks nullptr

#define ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT

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
    uint32_t get_graph_queue_family() { return graph_queue_family; }
    VkQueue get_graph_queue() { return graph_queue; };
    VkCommandPool get_cmd_pool() { return cmd_pool; }
    VkFormat get_window_format() { return format; }
    VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkSampleCountFlagBits get_max_msaa_sample_counts() { return max_msaa_sample_counts; }

    void allocate_cmd_buffer(VkCommandBufferLevel level, VkCommandBuffer *p_cmd_buffer);
    void free_cmd_buffer(VkCommandBuffer cmd_buffer);

protected:
    void _initialize_window_arguments(VkSurfaceKHR surface);

private:
#ifdef ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT
        PFN_vkCreateDebugUtilsMessengerEXT fnCreateDebugUtilsMessengerEXT = VK_NULL_HANDLE;
        PFN_vkDestroyDebugUtilsMessengerEXT fnDestroyDebugUtilsMessengerExt = VK_NULL_HANDLE;
#endif

    void _load_proc_addr();
    void _create_device();
    void _create_cmd_pool();
    void _create_vma_allocator();

    VkInstance instance = VK_NULL_HANDLE;
#ifdef ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
#endif
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceFeatures physical_device_features;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t graph_queue_family;
    VkQueue graph_queue = VK_NULL_HANDLE;
    VkCommandPool cmd_pool = VK_NULL_HANDLE;
    VmaAllocator allocator = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR capabilities;
    VkFormat format;
    VkSampleCountFlagBits max_msaa_sample_counts = VK_SAMPLE_COUNT_1_BIT;
};

#endif /* _RENDERING_CONTEXT_DRIVER_VULKAN_H */
