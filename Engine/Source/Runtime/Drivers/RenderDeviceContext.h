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

#include <Bright/Error.h>
#include <Bright/Typedefs.h>
#include <time.h>
#include <vector>

#define VK_NONE_FLAGS 0

#define ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT

#include "VulkanUtils.h"

// Render context driver of vulkan
class RenderDeviceContext {
public:
    RenderDeviceContext();
    ~RenderDeviceContext();

    Error Initialize();
    VkInstance GetInstance() { return instance; }
    VkPhysicalDevice GetPhysicalDevice() { return physical_device; }
    const char *GetDeviceName() { return physical_device_properties.deviceName; }
    VkDevice GetDevice() { return device; }
    VmaAllocator GetAllocator() { return allocator; }
    uint32_t GetQueueFamily() { return graph_queue_family; }
    VkQueue GetQueue() { return graph_queue; };
    VkCommandPool GetCommandPool() { return cmd_pool; }
    VkFormat GetWindowFormat() { return format; }
    VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkSampleCountFlagBits GetMaxMSAASampleCounts() { return max_msaa_sample_counts; }

    void AllocateCommandBuffer(VkCommandBufferLevel level, VkCommandBuffer *p_cmd_buffer);
    void FreeCommandBuffer(VkCommandBuffer cmd_buffer);

protected:
    void _InitializeWindowArguments(VkSurfaceKHR surface);

private:
#ifdef ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT
        PFN_vkCreateDebugUtilsMessengerEXT fnCreateDebugUtilsMessengerEXT = VK_NULL_HANDLE;
        PFN_vkDestroyDebugUtilsMessengerEXT fnDestroyDebugUtilsMessengerExt = VK_NULL_HANDLE;
#endif

    void _LoadVulkanFunctionProcAddr();
    void _CreateDevice();
    void _CreateCommandPool();
    void _CreateVmaAllocator();

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
