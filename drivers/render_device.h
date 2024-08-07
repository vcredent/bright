/* ======================================================================== */
/* render_device.h                                                          */
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
#ifndef _RENDERING_DEVICE_DRIVER_VULKAN_H
#define _RENDERING_DEVICE_DRIVER_VULKAN_H

#include "render_device_context.h"
#include <vector>

class RenderDevice {
public:
    RenderDevice(RenderDeviceContext *driver_context);
    ~RenderDevice();

    RenderDeviceContext *get_device_context() { return vk_rdc; }
    VkDescriptorPool get_descriptor_pool() { return descriptor_pool; }
    VkFormat get_surface_format() { return vk_rdc->get_window_format(); }
    VkSampleCountFlagBits get_msaa_samples() { return msaa_sample_counts; }

    struct Buffer {
        VkBuffer vk_buffer;
        VkDeviceSize size;
        VmaAllocation allocation;
        VmaAllocationInfo allocation_info;
    };

    Buffer *create_buffer(VkBufferUsageFlags usage, VkDeviceSize size);
    void destroy_buffer(Buffer *p_buffer);
    void write_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);
    void read_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);

    void create_render_pass(uint32_t attachment_count, VkAttachmentDescription *p_attachments, uint32_t subpass_count, VkSubpassDescription *p_subpass, uint32_t dependency_count, VkSubpassDependency *p_dependencies, VkRenderPass *p_render_pass);
    void destroy_render_pass(VkRenderPass render_pass);
    void allocate_cmd_buffer(VkCommandBuffer *p_cmd_buffer);
    void free_cmd_buffer(VkCommandBuffer cmd_buffer);

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
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageAspectFlags aspect_mask;
        size_t size = 0;
    };

    struct TextureCreateInfo {
        uint32_t width;
        uint32_t height;
        VkSampleCountFlagBits samples;
        VkFormat format;
        VkImageAspectFlags aspect_mask;
        VkImageType image_type;
        VkImageViewType image_view_type;
        VkImageUsageFlags usage;
    };

    Texture2D *create_texture(TextureCreateInfo *p_create_info);
    void destroy_texture(Texture2D *p_texture);
    void write_texture(Texture2D *texture, size_t size, void *pixels);
    void create_framebuffer(uint32_t width, uint32_t height, uint32_t image_view_count, VkImageView *p_image_view, VkRenderPass render_pass, VkFramebuffer *p_framebuffer);
    void destroy_framebuffer(VkFramebuffer framebuffer);

    struct SamplerCreateInfo {
        VkSamplerAddressMode u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode w = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkBorderColor border_color = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    };

    void create_sampler(SamplerCreateInfo* p_create_info, VkSampler *p_sampler);
    void destroy_sampler(VkSampler sampler);
    void bind_texture_sampler(Texture2D *texture, VkSampler sampler);

    void create_descriptor_set_layout(uint32_t bind_count, VkDescriptorSetLayoutBinding *p_bind, VkDescriptorSetLayout *p_descriptor_set_layout);
    void destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout);
    void allocate_descriptor_set(VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet *p_descriptor_set);
    void free_descriptor_set(VkDescriptorSet descriptor_set);
    void update_descriptor_set_buffer(Buffer *p_buffer, uint32_t binding, VkDescriptorSet descriptor_set);
    void update_descriptor_set_image(Texture2D *p_texture, uint32_t binding, VkDescriptorSet descriptor_set);

    struct ShaderInfo {
        const char *vertex = NULL;
        const char *fragment = NULL;
        uint32_t attribute_count = 0;
        VkVertexInputAttributeDescription *attributes = NULL;
        uint32_t bind_count = 0;
        VkVertexInputBindingDescription *binds = NULL;
        uint32_t descriptor_set_layout_count = 0;
        VkDescriptorSetLayout *p_descriptor_set_layouts = NULL;
        uint32_t push_const_count = 0;
        VkPushConstantRange *p_push_const_range = NULL;
    };

    struct ComputeShaderInfo {
        const char *compute = NULL;
        uint32_t descriptor_set_layout_count = 0;
        VkDescriptorSetLayout *p_descriptor_set_layouts = NULL;
        uint32_t push_const_count = 0;
        VkPushConstantRange *p_push_const_range = NULL;
    };

    struct PipelineCreateInfo {
        VkRenderPass render_pass;
        VkPolygonMode polygon;
        VkPrimitiveTopology topology;
        VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
        float line_width = 1.0f;
        VkBool32 blend_enable = VK_FALSE;
        VkBlendFactor src_color_blend_factor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor dst_color_blend_factor = VK_BLEND_FACTOR_ZERO;
    };

    struct Pipeline {
        VkPipeline pipeline;
        VkPipelineLayout layout;
        VkPipelineBindPoint bind_point;
    };

    Pipeline *create_graphics_pipeline(PipelineCreateInfo *p_create_info, ShaderInfo *p_shader_info);
    Pipeline *create_compute_pipeline(ComputeShaderInfo *p_shader_info);
    void destroy_pipeline(Pipeline *p_pipeline);

    void cmd_buffer_begin(VkCommandBuffer cmd_buffer, VkCommandBufferUsageFlags usage);
    void cmd_buffer_end(VkCommandBuffer cmd_buffer);
    void cmd_buffer_one_time_begin(VkCommandBuffer *p_cmd_buffer);
    void cmd_buffer_one_time_end(VkCommandBuffer cmd_buffer);

    struct PipelineMemoryBarrier {
        struct {
            Texture2D *texture = VK_NULL_HANDLE;
            VkImageLayout old_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkImageLayout new_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkAccessFlags src_access_mask = 0;
            VkAccessFlags dst_access_mask = 0;
        } image;
    };

    void cmd_pipeline_barrier(VkCommandBuffer cmd_buffer, const PipelineMemoryBarrier *p_pipeline_memory_barrier);

    void cmd_begin_render_pass(VkCommandBuffer cmd_buffer, VkRenderPass render_pass, uint32_t clear_value_count, VkClearValue *p_clear_values, VkFramebuffer framebuffer, VkRect2D *p_rect);
    void cmd_end_render_pass(VkCommandBuffer cmd_buffer);
    void cmd_bind_vertex_buffer(VkCommandBuffer cmd_buffer, Buffer *p_buffer);
    void cmd_bind_index_buffer(VkCommandBuffer cmd_buffer, VkIndexType type, Buffer *p_buffer);
    void cmd_draw(VkCommandBuffer cmd_buffer, uint32_t vertex_count);
    void cmd_draw_indexed(VkCommandBuffer cmd_buffer, uint32_t index_count);
    void cmd_bind_pipeline(VkCommandBuffer cmd_buffer, Pipeline *p_pipeline);
    void cmd_buffer_submit(VkCommandBuffer cmd_buffer, uint32_t wait_semaphore_count, VkSemaphore *p_wait_semaphore, uint32_t signal_semaphore_count, VkSemaphore *p_signal_semaphore, VkPipelineStageFlags *p_mask, VkQueue queue, VkFence fence);
    void cmd_bind_descriptor_set(VkCommandBuffer cmd_buffer, Pipeline *p_pipeline, VkDescriptorSet descriptor);
    void cmd_setval_viewport(VkCommandBuffer cmd_buffer , uint32_t w, uint32_t h);
    void cmd_push_const(VkCommandBuffer cmd_buffer, RenderDevice::Pipeline *pipeline, VkShaderStageFlags shader_stage_flags, uint32_t offset, uint32_t size, void *p_values);
    void present(VkQueue queue, VkSwapchainKHR swap_chain, uint32_t index, VkSemaphore wait_semaphore);

private:
    void _initialize_descriptor_pool();

    RenderDeviceContext *vk_rdc;
    VkDevice vk_device;
    VmaAllocator allocator;
    VkDescriptorPool descriptor_pool;
    VkSampleCountFlagBits msaa_sample_counts;
};

#endif /* _RENDERING_DEVICE_DRIVER_VULKAN_H */