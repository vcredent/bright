/* ======================================================================== */
/* vulkan_utils.h                                                           */
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
#ifndef _VULKAN_UTILS_H
#define _VULKAN_UTILS_H

#include <fstream>
#include <stdexcept>
#include <copilot/ioutils.h>

// pick surface format
static VkSurfaceFormatKHR pick_surface_format(const VkSurfaceFormatKHR *surface_formats, uint32_t count)
{
    VkFormat format;
    for (uint32_t i = 0; i < count; i++) {
        format = surface_formats[i].format;
        if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM ||
            format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 || format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 ||
            format == VK_FORMAT_R16G16B16A16_SFLOAT)
            return surface_formats[i];
    }

    printf("Can't find our preferred formats... Falling back to first exposed format. Rendering may be incorrect.\n");

    assert(count >= 1);
    return surface_formats[0];
}

static VkSampleCountFlagBits find_max_msaa_sample_counts(VkPhysicalDeviceProperties properties)
{
    VkSampleCountFlags framebuffer_color_sample_count;
    VkSampleCountFlags framebuffer_depth_sample_count;

    framebuffer_color_sample_count = properties.limits.framebufferColorSampleCounts;
    framebuffer_depth_sample_count = properties.limits.framebufferDepthSampleCounts;

    VkSampleCountFlags counts =
            std::min(framebuffer_color_sample_count, framebuffer_depth_sample_count);

    if (counts & VK_SAMPLE_COUNT_64_BIT)
        return VK_SAMPLE_COUNT_64_BIT;

    if (counts & VK_SAMPLE_COUNT_32_BIT)
        return VK_SAMPLE_COUNT_32_BIT;

    if (counts & VK_SAMPLE_COUNT_16_BIT)
        return VK_SAMPLE_COUNT_16_BIT;

    if (counts & VK_SAMPLE_COUNT_8_BIT)
        return VK_SAMPLE_COUNT_8_BIT;

    if (counts & VK_SAMPLE_COUNT_4_BIT)
        return VK_SAMPLE_COUNT_4_BIT;

    if (counts & VK_SAMPLE_COUNT_2_BIT)
        return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

// load shader module form .spv file content.
static VkShaderModule load_shader_module(VkDevice device, const char *name, const char *stage)
{
    char *buf;
    size_t size;
    VkResult U_ASSERT_ONLY err;

    char path[255];
    snprintf(path, sizeof(path), _CURDIR("shader/%s.%s.spv"), name, stage);

    buf = io_read_bytecode(path, &size);

    VkShaderModuleCreateInfo shader_module_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ 0,
            /* codeSize */ size,
            /* pCode */ reinterpret_cast<uint32_t *>(buf),
    };

    VkShaderModule shader_module;
    err = vkCreateShaderModule(device, &shader_module_create_info, allocation_callbacks, &shader_module);
    assert(!err);

    io_free_buf(buf);

    return shader_module;
}

#endif /* _VULKAN_UTILS_H */
