/* ======================================================================== */
/* rendering_device_driver_vulkan.h                                         */
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
#ifndef _RENDERING_DEVICE_DRIVER_VULKAN_H
#define _RENDERING_DEVICE_DRIVER_VULKAN_H

#include "rendering_context_driver_vulkan.h"

class RenderingDeviceDriverVulkan {
public:
    RenderingDeviceDriverVulkan(RenderingContextDriverVulkan *driver_context);
    ~RenderingDeviceDriverVulkan();

    struct Buffer {
        VkBuffer vk_buffer;
        VkDeviceSize size;
        VmaAllocation allocation;
        VmaAllocationInfo allocation_info;
    };

    Buffer *create_buffer(VkDeviceSize size);
    void destroy_buffer(Buffer *p_buffer);
    void write_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);
    void read_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);

    void create_graph_pipeline(const char *vertex_shader, const char *fragment_shader,
                               uint32_t bind_count,
                               VkVertexInputBindingDescription *p_bind,
                               uint32_t attribute_count,
                               VkVertexInputAttributeDescription *p_attribute,
                               VkPipeline *p_pipeline);

    void begin_graph_command_buffer(VkCommandBuffer *p_command_buffer);
    void end_graph_command_buffer(VkCommandBuffer command_buffer);
    void command_bind_vertex_buffer(Buffer *p_buffer);
    void command_bind_graph_pipeline(VkCommandBuffer command_buffer, VkPipeline pipeline);

private:
    void _initialize_descriptor_pool();

    RenderingContextDriverVulkan *vk_driver_context;
    VkDevice vk_device;
    VmaAllocator allocator;
    VkDescriptorPool descriptor_pool;
    VkCommandBuffer graph_command_buffer;
};

#endif /* _RENDERING_DEVICE_DRIVER_VULKAN_H */