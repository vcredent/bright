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
#include <vector>

class RenderingDeviceDriverVulkan {
public:
    RenderingDeviceDriverVulkan(RenderingContextDriverVulkan *driver_context);
    ~RenderingDeviceDriverVulkan();

public:
    struct Buffer {
        VkBuffer vk_buffer;
        VkDeviceSize size;
        VmaAllocation allocation;
        VmaAllocationInfo allocation_info;
    };

    struct ShaderInfo {
        const char *vertex;
        const char *fragment;
        uint32_t attribute_count;
        VkVertexInputAttributeDescription *attributes;
        uint32_t bind_count;
        VkVertexInputBindingDescription *binds;
        uint32_t descriptor_count;
        VkDescriptorSetLayout *descriptor_set_layouts;
    };

    struct Pipeline {
        VkPipeline pipeline;
        VkPipelineLayout layout;
        VkPipelineBindPoint bind_point;
    };

public:
    Buffer *create_buffer(VkBufferUsageFlags usage, VkDeviceSize size);
    void destroy_buffer(Buffer *p_buffer);
    void write_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);
    void read_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);

    void create_descriptor_set_layout(uint32_t bind_count, VkDescriptorSetLayoutBinding *p_bind, VkDescriptorSetLayout *p_descriptor_set_layout);
    void destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout);
    void allocate_descriptor(VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet *p_descriptor);
    void free_descriptor(VkDescriptorSet descriptor);
    void write_descriptor_set(Buffer *p_buffer, VkDescriptorSet descriptor_set);

    Pipeline *create_graph_pipeline(ShaderInfo *p_shader_attribute);
    void destroy_pipeline(Pipeline *p_pipeline);

    void command_buffer_begin(VkCommandBuffer *p_command_buffer);
    void command_buffer_end(VkCommandBuffer command_buffer);
    void command_begin_render_pass(VkCommandBuffer command_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkRect2D *p_rect);
    void command_end_render_pass(VkCommandBuffer command_buffer);
    void command_bind_vertex_buffer(Buffer *p_buffer);
    void command_bind_graph_pipeline(VkCommandBuffer command_buffer, Pipeline *p_pipeline);
    void command_buffer_submit(VkCommandBuffer command_buffer, uint32_t wait_semaphore_count, VkSemaphore *p_wait_semaphore, uint32_t signal_semaphore_count, VkSemaphore *p_signal_semaphore, VkPipelineStageFlags *p_mask, VkQueue queue, VkFence fence);
    void command_bind_descriptor(VkCommandBuffer command_buffer, Pipeline *p_pipeline, VkDescriptorSet descriptor);
    void present(VkQueue queue, VkSwapchainKHR swap_chain, uint32_t index, VkSemaphore wait_semaphore);

private:
    void _initialize_descriptor_pool();

    RenderingContextDriverVulkan *vk_driver_context;
    VkDevice vk_device;
    VmaAllocator allocator;
    VkDescriptorPool descriptor_pool;
    VkCommandBuffer graph_command_buffer;
};

#endif /* _RENDERING_DEVICE_DRIVER_VULKAN_H */