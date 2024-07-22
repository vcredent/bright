/* ======================================================================== */
/* render_device.cpp                                                        */
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
#include "render_device.h"

RenderDevice::RenderDevice(RenderDeviceContext *driver_context)
    : vk_rdc(driver_context)
{
    vk_device = vk_rdc->get_device();
    allocator = vk_rdc->get_allocator();

    _initialize_descriptor_pool();
}

RenderDevice::~RenderDevice()
{
    vkDestroyDescriptorPool(vk_device, descriptor_pool, allocation_callbacks);
}

RenderDevice::Buffer *RenderDevice::create_buffer(VkBufferUsageFlags usage, VkDeviceSize size)
{
    VkResult U_ASSERT_ONLY err;

    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.usage = usage;
    buffer_create_info.size = size;

    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    Buffer *buffer = (Buffer *) imalloc(sizeof(Buffer));
    buffer->size = size;

    err = vmaCreateBuffer(allocator, &buffer_create_info, &allocation_create_info, &buffer->vk_buffer, &buffer->allocation, &buffer->allocation_info);
    assert(!err);

    return buffer;
}

void RenderDevice::destroy_buffer(Buffer *p_buffer)
{
    vmaDestroyBuffer(allocator, p_buffer->vk_buffer, p_buffer->allocation);
    free(p_buffer);
}

void RenderDevice::write_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf)
{
    char *tmp;
    vmaMapMemory(allocator, buffer->allocation, (void **) &tmp);
    memcpy((tmp + offset), buf, size);
    vmaUnmapMemory(allocator, buffer->allocation);
}

void
RenderDevice::read_buffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf)
{
    char *tmp;
    vmaMapMemory(allocator, buffer->allocation, (void **) &tmp);
    memcpy(buf, (tmp + offset), size);
    vmaUnmapMemory(allocator, buffer->allocation);
}

void RenderDevice::create_render_pass(uint32_t attachment_count, VkAttachmentDescription *p_attachments, uint32_t subpass_count, VkSubpassDescription *p_subpass, uint32_t dependency_count, VkSubpassDependency *p_dependencies, VkRenderPass *p_render_pass)
{
    VkResult U_ASSERT_ONLY err;

    // create render pass
    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = attachment_count;
    render_pass_create_info.pAttachments = p_attachments;
    render_pass_create_info.subpassCount = subpass_count;
    render_pass_create_info.pSubpasses = p_subpass;
    render_pass_create_info.dependencyCount = dependency_count;
    render_pass_create_info.pDependencies = p_dependencies;

    err = vkCreateRenderPass(vk_device, &render_pass_create_info, allocation_callbacks, p_render_pass);
    assert(!err);
}

void RenderDevice::destroy_render_pass(VkRenderPass render_pass)
{
    vkDestroyRenderPass(vk_device, render_pass, allocation_callbacks);
}

void RenderDevice::allocate_command_buffer(VkCommandBuffer *p_command_buffer)
{
    vk_rdc->allocate_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, p_command_buffer);
}

void RenderDevice::free_command_buffer(VkCommandBuffer command_buffer)
{
    vk_rdc->free_command_buffer(command_buffer);
}

RenderDevice::Texture2D *RenderDevice::create_texture(uint32_t width, uint32_t height, VkSampler sampler, VkFormat format, VkImageUsageFlags usage)
{
    VkResult U_ASSERT_ONLY err;
    Texture2D *texture;

    texture = (Texture2D *) imalloc(sizeof(Texture2D));
    texture->format = format;
    texture->width = width;
    texture->height = height;
    texture->sampler = sampler;

    VkImageCreateInfo image_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* imageType */ VK_IMAGE_TYPE_2D,
            /* format */ texture->format,
            /* extent */ { width, height, 1 },
            /* mipLevels */ 1,
            /* arrayLayers */ 1,
            /* samples */ VK_SAMPLE_COUNT_1_BIT,
            /* tiling */ VK_IMAGE_TILING_OPTIMAL,
            /* usage */ usage,
            /* sharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 0,
            /* pQueueFamilyIndices */ nullptr,
            /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    err = vmaCreateImage(allocator, &image_create_info, &allocation_create_info, &texture->image, &texture->allocation, &texture->allocation_info);
    assert(!err);

    VkImageViewCreateInfo image_view_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* image */ texture->image,
            /* viewType */ VK_IMAGE_VIEW_TYPE_2D,
            /* format */ texture->format,
            /* components */
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
            /* subresourceRange */
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
    };

    err = vkCreateImageView(vk_device, &image_view_create_info, allocation_callbacks, &texture->image_view);
    assert(!err);

    return texture;
}

void RenderDevice::destroy_texture(Texture2D *p_texture)
{
    vmaDestroyImage(allocator, p_texture->image, p_texture->allocation);
    vkDestroyImageView(vk_device, p_texture->image_view, allocation_callbacks);
    if (p_texture->descriptor_set)
        free_descriptor_set(p_texture->descriptor_set);
}

void
RenderDevice::create_framebuffer(uint32_t width, uint32_t height, uint32_t image_view_count, VkImageView *p_image_view, VkRenderPass render_pass, VkFramebuffer *p_framebuffer)
{
    VkFramebufferCreateInfo framebuffer_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* renderPass */ render_pass,
            /* attachmentCount */ image_view_count,
            /* pAttachments */ p_image_view,
            /* width */ width,
            /* height */ height,
            /* layers */ 1,
    };

    vkCreateFramebuffer(vk_device, &framebuffer_create_info, allocation_callbacks, p_framebuffer);
}

void RenderDevice::destroy_framebuffer(VkFramebuffer framebuffer)
{
    vkDestroyFramebuffer(vk_device, framebuffer, allocation_callbacks);
}

void RenderDevice::create_sampler(VkSampler *p_sampler)
{
    VkSamplerCreateInfo sampler_create_info = {};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.anisotropyEnable = VK_FALSE;
    sampler_create_info.maxAnisotropy = 16;
    sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_create_info.unnormalizedCoordinates = VK_FALSE;
    sampler_create_info.compareEnable = VK_FALSE;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.minLod = 0.0f;
    sampler_create_info.maxLod = 0.0f;

    vkCreateSampler(vk_device, &sampler_create_info, allocation_callbacks, p_sampler);
}

void RenderDevice::destroy_sampler(VkSampler sampler)
{
    vkDestroySampler(vk_device, sampler, allocation_callbacks);
}

void RenderDevice::transition_image_layout(Texture2D *p_texture, VkImageLayout new_layout)
{
    VkCommandBuffer one_time_cmd_buffer;
    allocate_command_buffer(&one_time_cmd_buffer);
    command_buffer_begin(one_time_cmd_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = p_texture->image_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = p_texture->image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    if (p_texture->image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        goto pipeline_barrier_create_end_tag;
    }

    if (p_texture->image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        goto pipeline_barrier_create_end_tag;
    }

    if (p_texture->image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_HOST_READ_BIT;
        goto pipeline_barrier_create_end_tag;
    }

    throw std::invalid_argument("unsupported layout transition!");

    pipeline_barrier_create_end_tag:
    vkCmdPipelineBarrier(
            one_time_cmd_buffer,
            sourceStage,
            destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    command_buffer_end(one_time_cmd_buffer);
    free_command_buffer(one_time_cmd_buffer);

    p_texture->image_layout = new_layout;
}

void
RenderDevice::create_descriptor_set_layout(uint32_t bind_count, VkDescriptorSetLayoutBinding *p_bind, VkDescriptorSetLayout *p_descriptor_set_layout)
{
    VkResult U_ASSERT_ONLY err;

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* bindingCount */ bind_count,
            /* pBindings */ p_bind,
    };

    err = vkCreateDescriptorSetLayout(vk_device, &descriptor_set_layout_create_info, allocation_callbacks, p_descriptor_set_layout);
    assert(!err);
}

void RenderDevice::destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout)
{
    vkDestroyDescriptorSetLayout(vk_device, descriptor_set_layout, allocation_callbacks);
}

void
RenderDevice::allocate_descriptor_set(VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet *p_descriptor_set)
{
    VkDescriptorSetAllocateInfo descriptor_allocate_info = {
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            /* pNext */ nextptr,
            /* descriptorPool */ descriptor_pool,
            /* descriptorSetCount */ 1,
            /* pSetLayouts */ &descriptor_set_layout,
    };

    vkAllocateDescriptorSets(vk_device, &descriptor_allocate_info, p_descriptor_set);
}

void RenderDevice::free_descriptor_set(VkDescriptorSet descriptor_set)
{
    vkFreeDescriptorSets(vk_device, descriptor_pool, 1, &descriptor_set);
}

void RenderDevice::write_descriptor_set(Buffer *p_buffer, VkDescriptorSet descriptor_set)
{
    VkDescriptorBufferInfo buffer_info = {
            /* buffer */ p_buffer->vk_buffer,
            /* offset */ 0,
            /* range */ p_buffer->size,
    };

    VkWriteDescriptorSet write_info = {
            /* sType */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /* pNext */ nextptr,
            /* dstSet */ descriptor_set,
            /* dstBinding */ 0,
            /* dstArrayElement */ 0,
            /* descriptorCount */ 1,
            /* descriptorType */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /* pImageInfo */ VK_NULL_HANDLE,
            /* pBufferInfo */ &buffer_info,
            /* pTexelBufferView */ VK_NULL_HANDLE,
    };

    vkUpdateDescriptorSets(vk_device, 1, &write_info, 0, nullptr);
}

RenderDevice::Pipeline *RenderDevice::create_graph_pipeline(ShaderInfo *p_shader_info)
{
    VkResult U_ASSERT_ONLY err;

    VkPipelineLayoutCreateInfo pipeline_layout_creat_info = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* setLayoutCount */ p_shader_info->descriptor_count,
            /* pSetLayouts */ p_shader_info->descriptor_set_layouts,
            /* pushConstantRangeCount */ 0,
            /* pPushConstantRanges */ nullptr,
    };

    VkPipelineLayout vk_pipeline_layout;
    err = vkCreatePipelineLayout(vk_device, &pipeline_layout_creat_info, allocation_callbacks, &vk_pipeline_layout);
    assert(!err);

    VkShaderModule vertex_shader_module, fragment_shader_module;

    vertex_shader_module = load_shader_module(vk_device, p_shader_info->vertex);
    fragment_shader_module = load_shader_module(vk_device, p_shader_info->fragment);

    VkPipelineShaderStageCreateInfo vertex_shader_create_info = {};
    vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_create_info.module = vertex_shader_module;
    vertex_shader_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_shader_create_info = {};
    fragment_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_create_info.module = fragment_shader_module;
    fragment_shader_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages_info[] = {
            vertex_shader_create_info, fragment_shader_create_info
    };

    VkPipelineVertexInputStateCreateInfo input_state_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* vertexBindingDescriptionCount */ p_shader_info->bind_count,
            /* pVertexBindingDescriptions */ p_shader_info->binds,
            /* vertexAttributeDescriptionCount */ p_shader_info->attribute_count,
            /* pVertexAttributeDescriptions */ p_shader_info->attributes,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* topology */ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            /* primitiveRestartEnable */ VK_FALSE,
    };

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = vk_rdc->get_width();
    viewport.height = vk_rdc->get_height();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = vk_rdc->get_width();
    scissor.extent.height = vk_rdc->get_height();

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* viewportCount */ 1,
            /* pViewports */ &viewport,
            /* scissorCount */ 1,
            /* pScissors */ &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
    rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_create_info.depthClampEnable = VK_FALSE;
    rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_create_info.lineWidth = 1.0f;
    rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_create_info.depthBiasEnable = VK_FALSE;
    rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_create_info.depthBiasClamp = 0.0f;
    rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
    multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state_create_info.minSampleShading = 1.0f;
    multisample_state_create_info.pSampleMask = nullptr;
    multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
    multisample_state_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
    color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.blendEnable = VK_TRUE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
    color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_create_info.logicOpEnable = VK_FALSE;
    color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
    color_blend_state_create_info.blendConstants[0] = 0.0f;
    color_blend_state_create_info.blendConstants[1] = 0.0f;
    color_blend_state_create_info.blendConstants[2] = 0.0f;
    color_blend_state_create_info.blendConstants[3] = 0.0f;

    VkDynamicState dynamic_state[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_crate_info = {
            /* sType= */ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            /* pNext= */ nextptr,
            /* flags= */ no_flag_bits,
            /* dynamicStateCount= */ ARRAY_SIZE(dynamic_state),
            /* pDynamicStates= */ dynamic_state,
    };

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* stageCount */ ARRAY_SIZE(shader_stages_info),
            /* pStages */ shader_stages_info,
            /* pVertexInputState */ &input_state_create_info,
            /* pInputAssemblyState */ &input_assembly,
            /* pTessellationState */ nullptr,
            /* pViewportState */ &viewport_state_create_info,
            /* pRasterizationState */ &rasterization_state_create_info,
            /* pMultisampleState */ &multisample_state_create_info,
            /* pDepthStencilState */ nullptr,
            /* pColorBlendState */ &color_blend_state_create_info,
            /* pDynamicState */ &dynamic_state_crate_info,
            /* layout */ vk_pipeline_layout,
            /* renderPass */ vk_rdc->get_render_pass(),
            /* subpass */ 0,
            /* basePipelineHandle */ VK_NULL_HANDLE,
            /* basePipelineIndex */ -1,
    };

    VkPipeline vk_pipeline;
    err = vkCreateGraphicsPipelines(vk_device, nullptr, 1, &pipeline_create_info, allocation_callbacks, &vk_pipeline);
    assert(!err);

    Pipeline *p_pipeline = (Pipeline*) imalloc(sizeof(Pipeline));
    p_pipeline->pipeline = vk_pipeline;
    p_pipeline->layout = vk_pipeline_layout;
    p_pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    vkDestroyShaderModule(vk_device, vertex_shader_module, allocation_callbacks);
    vkDestroyShaderModule(vk_device, fragment_shader_module, allocation_callbacks);

    return p_pipeline;
}

void RenderDevice::_initialize_descriptor_pool()
{
    VkResult U_ASSERT_ONLY err;

    VkDescriptorPoolSize pool_size[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER,                256 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          256 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          256 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   256 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   256 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         256 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         256 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 256 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 256 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       256 }
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            /* maxSets */ 1024,
            /* poolSizeCount */ ARRAY_SIZE(pool_size),
            /* pPoolSizes */ pool_size,
    };

    err = vkCreateDescriptorPool(vk_device, &descriptor_pool_create_info, allocation_callbacks, &descriptor_pool);
    assert(!err);
}

void RenderDevice::destroy_pipeline(Pipeline *p_pipeline)
{
    vkDestroyPipelineLayout(vk_device, p_pipeline->layout, allocation_callbacks);
    vkDestroyPipeline(vk_device, p_pipeline->pipeline, allocation_callbacks);
}

void RenderDevice::command_buffer_begin(VkCommandBuffer command_buffer, VkCommandBufferUsageFlags usage)
{
    VkCommandBufferBeginInfo command_buffer_begin_info = {
            /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            /* pNext */ nextptr,
            /* flags */ usage,
            /* pInheritanceInfo */ nullptr,
    };
    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
}

void RenderDevice::command_buffer_end(VkCommandBuffer command_buffer)
{
    vkEndCommandBuffer(command_buffer);
}

void RenderDevice::command_begin_render_pass(VkCommandBuffer command_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkRect2D *p_rect)
{
    VkClearValue clear_color = {
        0.0f, 0.0f, 0.0f, 1.0f
    };

    VkRenderPassBeginInfo render_pass_begin_info = {
            /* sType */ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            /* pNext */ nextptr,
            /* renderPass */ render_pass,
            /* framebuffer */ framebuffer,
            /* renderArea */ *p_rect,
            /* clearValueCount */ 1,
            /* pClearValues */ &clear_color,
    };

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderDevice::command_end_render_pass(VkCommandBuffer command_buffer)
{
    vkCmdEndRenderPass(command_buffer);
}

void RenderDevice::command_bind_graph_pipeline(VkCommandBuffer command_buffer, Pipeline *p_pipeline)
{
    vkCmdBindPipeline(command_buffer, p_pipeline->bind_point, p_pipeline->pipeline);
}

void RenderDevice::command_buffer_submit(VkCommandBuffer command_buffer, uint32_t wait_semaphore_count, VkSemaphore *p_wait_semaphore, uint32_t signal_semaphore_count, VkSemaphore *p_signal_semaphore, VkPipelineStageFlags *p_mask, VkQueue queue, VkFence fence)
{
    VkResult U_ASSERT_ONLY err;

    uint32_t command_buffer_count;
    VkCommandBuffer command_buffers[] = { command_buffer };
    command_buffer_count = command_buffer ? ARRAY_SIZE(command_buffers) : 0;

    VkSubmitInfo submit_info = {
            /* sType */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
            /* pNext */ nextptr,
            /* waitSemaphoreCount */ wait_semaphore_count,
            /* pWaitSemaphores */ p_wait_semaphore,
            /* pWaitDstStageMask */ p_mask,
            /* commandBufferCount */ command_buffer_count,
            /* pCommandBuffers */ command_buffers,
            /* signalSemaphoreCount */ signal_semaphore_count,
            /* pSignalSemaphores */ p_signal_semaphore,
    };

    err = vkQueueSubmit(queue, 1, &submit_info, fence);
    assert(!err);
}

void RenderDevice::command_bind_descriptor_set(VkCommandBuffer command_buffer, Pipeline *p_pipeline, VkDescriptorSet descriptor)
{
    vkCmdBindDescriptorSets(command_buffer, p_pipeline->bind_point, p_pipeline->layout, 0, 1, &descriptor, 0, VK_NULL_HANDLE);
}

void RenderDevice::command_setval_viewport(VkCommandBuffer command_buffer, uint32_t w, uint32_t h)
{
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = w;
    viewport.height = h;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { w, h };
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void RenderDevice::present(VkQueue queue, VkSwapchainKHR swap_chain, uint32_t index, VkSemaphore wait_semaphore)
{
    VkPresentInfoKHR present_info = {
            /* sType */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            /* pNext */ nextptr,
            /* waitSemaphoreCount */ 1,
            /* pWaitSemaphores */ &wait_semaphore,
            /* swapchainCount */ 1,
            /* pSwapchains */ &swap_chain,
            /* pImageIndices */ &index,
            /* pResults */ VK_NULL_HANDLE,
    };

    vkQueuePresentKHR(queue, &present_info);
    vkQueueWaitIdle(queue);
}