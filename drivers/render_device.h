/* ======================================================================== */
/* render_device.h                                                          */
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
#ifndef _RENDERING_DEVICE_DRIVER_VULKAN_H
#define _RENDERING_DEVICE_DRIVER_VULKAN_H

#include "render_device_context.h"
#include <vector>

class RenderDevice {
public:
    RenderDevice(RenderDeviceContext *driver_context);
    ~RenderDevice();

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

    struct Texture2D {
        VkImage image;
        VkImageView image_view;
        VkImageLayout image_layout;
        VkDescriptorSet descriptor_set;
        VmaAllocation allocation;
        VmaAllocationInfo allocation_info;
        uint32_t width;
        uint32_t height;
        VkFormat format;
        VkSampler sampler;
    };

public:
    RenderDeviceContext *get_device_context() { return vk_rdc; }
    VkDescriptorPool get_descriptor_pool() { return descriptor_pool; }
    VkFormat get_surface_format() { return vk_rdc->get_window_format(); }

    Buffer *create_buffer(VkBufferUsageFlags usage, VkDeviceSize size);
    void destroy_buffer(Buffer *p_buffer);
    void write_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);
    void read_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);
    void create_render_pass(uint32_t attachment_count, VkAttachmentDescription *p_attachments, uint32_t subpass_count, VkSubpassDescription *p_subpass, uint32_t dependency_count, VkSubpassDependency *p_dependencies, VkRenderPass *p_render_pass);
    void destroy_render_pass(VkRenderPass render_pass);
    void allocate_cmd_buffer(VkCommandBuffer *p_cmd_buffer);
    void free_cmd_buffer(VkCommandBuffer cmd_buffer);
    Texture2D *create_texture(uint32_t width, uint32_t height, VkSampler sampler, VkFormat format, VkImageUsageFlags usage);
    void destroy_texture(Texture2D *p_texture);
    void create_framebuffer(uint32_t width, uint32_t height, uint32_t image_view_count, VkImageView *p_image_view, VkRenderPass render_pass, VkFramebuffer *p_framebuffer);
    void destroy_framebuffer(VkFramebuffer framebuffer);
    void create_sampler(VkSampler *p_sampler);
    void destroy_sampler(VkSampler sampler);
    void transition_image_layout(Texture2D *p_texture, VkImageLayout new_layout);

    void create_descriptor_set_layout(uint32_t bind_count, VkDescriptorSetLayoutBinding *p_bind, VkDescriptorSetLayout *p_descriptor_set_layout);
    void destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout);
    void allocate_descriptor_set(VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet *p_descriptor_set);
    void free_descriptor_set(VkDescriptorSet descriptor_set);
    void write_descriptor_set_buffer(Buffer *p_buffer, VkDescriptorSet descriptor_set);

    Pipeline *create_graph_pipeline(VkRenderPass render_pass, ShaderInfo *p_shader_attribute);
    void destroy_pipeline(Pipeline *p_pipeline);

    void cmd_buffer_begin(VkCommandBuffer cmd_buffer, VkCommandBufferUsageFlags usage);
    void cmd_buffer_end(VkCommandBuffer cmd_buffer);
    void cmd_begin_render_pass(VkCommandBuffer cmd_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkRect2D *p_rect);
    void cmd_end_render_pass(VkCommandBuffer cmd_buffer);
    void cmd_bind_vertex_buffer(VkCommandBuffer cmd_buffer, Buffer *p_buffer);
    void cmd_bind_index_buffer(VkCommandBuffer cmd_buffer, VkIndexType type, Buffer *p_buffer);
    void cmd_draw_indexed(VkCommandBuffer cmd_buffer, uint32_t index_count);
    void cmd_bind_graph_pipeline(VkCommandBuffer cmd_buffer, Pipeline *p_pipeline);
    void cmd_buffer_submit(VkCommandBuffer cmd_buffer, uint32_t wait_semaphore_count, VkSemaphore *p_wait_semaphore, uint32_t signal_semaphore_count, VkSemaphore *p_signal_semaphore, VkPipelineStageFlags *p_mask, VkQueue queue, VkFence fence);
    void cmd_bind_descriptor_set(VkCommandBuffer cmd_buffer, Pipeline *p_pipeline, VkDescriptorSet descriptor);
    void cmd_setval_viewport(VkCommandBuffer cmd_buffer , uint32_t w, uint32_t h);
    void present(VkQueue queue, VkSwapchainKHR swap_chain, uint32_t index, VkSemaphore wait_semaphore);

private:
    void _initialize_descriptor_pool();

    RenderDeviceContext *vk_rdc;
    VkDevice vk_device;
    VmaAllocator allocator;
    VkDescriptorPool descriptor_pool;
};

#endif /* _RENDERING_DEVICE_DRIVER_VULKAN_H */