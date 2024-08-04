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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
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

    msaa_sample_counts = vk_rdc->get_max_msaa_sample_counts();

    // if sample counts > 4x，that default msaa samples set 4x otherwise 2x
    msaa_sample_counts = msaa_sample_counts >= 4 ? VK_SAMPLE_COUNT_4_BIT : VK_SAMPLE_COUNT_2_BIT;
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

void RenderDevice::allocate_cmd_buffer(VkCommandBuffer *p_cmd_buffer)
{
    vk_rdc->allocate_cmd_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, p_cmd_buffer);
}

void RenderDevice::free_cmd_buffer(VkCommandBuffer cmd_buffer)
{
    vk_rdc->free_cmd_buffer(cmd_buffer);
}

RenderDevice::Texture2D *RenderDevice::create_texture(uint32_t width, uint32_t height, VkSampleCountFlagBits samples, VkFormat format, VkImageAspectFlags aspect_mask, VkImageUsageFlags usage)
{
    VkResult U_ASSERT_ONLY err;
    Texture2D *texture = VK_NULL_HANDLE;

    texture = (Texture2D *) imalloc(sizeof(Texture2D));
    texture->format = format;
    texture->width = width;
    texture->height = height;
    texture->aspect_mask = aspect_mask;

    VkImageCreateInfo image_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* imageType */ VK_IMAGE_TYPE_2D,
            /* format */ texture->format,
            /* extent */ { width, height, 1 },
            /* mipLevels */ 1,
            /* arrayLayers */ 1,
            /* samples */ samples,
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
                    .aspectMask = aspect_mask,
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
    VkResult U_ASSERT_ONLY err;

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

    err = vkCreateFramebuffer(vk_device, &framebuffer_create_info, allocation_callbacks, p_framebuffer);
    assert(!err);
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

void RenderDevice::bind_texture_sampler(RenderDevice::Texture2D *texture, VkSampler sampler)
{
    texture->sampler = sampler;
}

void RenderDevice::create_descriptor_set_layout(uint32_t bind_count, VkDescriptorSetLayoutBinding *p_bind, VkDescriptorSetLayout *p_descriptor_set_layout)
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

void RenderDevice::allocate_descriptor_set(VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet *p_descriptor_set)
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

void RenderDevice::update_descriptor_buffer(Buffer *p_buffer, uint32_t binding, VkDescriptorSet descriptor_set)
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
            /* dstBinding */ binding,
            /* dstArrayElement */ 0,
            /* descriptorCount */ 1,
            /* descriptorType */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /* pImageInfo */ VK_NULL_HANDLE,
            /* pBufferInfo */ &buffer_info,
            /* pTexelBufferView */ VK_NULL_HANDLE,
    };

    vkUpdateDescriptorSets(vk_device, 1, &write_info, 0, nullptr);
}

RenderDevice::Pipeline *RenderDevice::create_graphics_pipeline(PipelineCreateInfo *p_create_info, ShaderInfo *p_shader_info)
{
    VkResult U_ASSERT_ONLY err;

    VkPipelineLayoutCreateInfo pipeline_layout_creat_info = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* setLayoutCount */ p_shader_info->descriptor_set_layout_count,
            /* pSetLayouts */ p_shader_info->p_descriptor_set_layouts,
            /* pushConstantRangeCount */ p_shader_info->push_const_count,
            /* pPushConstantRanges */ p_shader_info->p_push_const_range,
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
            /* topology */ p_create_info->topology,
            /* primitiveRestartEnable */ VK_FALSE,
    };

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = 32;
    viewport.height = 32;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = 32;
    scissor.extent.height = 32;

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
    rasterization_state_create_info.polygonMode = p_create_info->polygon;
    rasterization_state_create_info.lineWidth = p_create_info->line_width;
    rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_create_info.depthBiasEnable = VK_FALSE;
    rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_create_info.depthBiasClamp = 0.0f;
    rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
    multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    multisample_state_create_info.rasterizationSamples = p_create_info->samples;
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

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.minDepthBounds = 0.0f;
    depth_stencil.maxDepthBounds = 1.0f;

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

    std::vector<VkDynamicState> dynamics = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };

    if (p_create_info->line_width > 1.0f)
        dynamics.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);

    VkPipelineDynamicStateCreateInfo dynamic_state_crate_info = {
            /* sType= */ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            /* pNext= */ nextptr,
            /* flags= */ no_flag_bits,
            /* dynamicStateCount= */ (uint32_t) std::size(dynamics),
            /* pDynamicStates= */ std::data(dynamics),
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
            /* pDepthStencilState */ &depth_stencil,
            /* pColorBlendState */ &color_blend_state_create_info,
            /* pDynamicState */ &dynamic_state_crate_info,
            /* layout */ vk_pipeline_layout,
            /* renderPass */ p_create_info->render_pass,
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
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       256 },
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

RenderDevice::Pipeline *RenderDevice::create_compute_pipeline(RenderDevice::ComputeShaderInfo *p_shader_info)
{
    Pipeline *pipeline = (Pipeline *) imalloc(sizeof(Pipeline));
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
            /* sType= */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            /* pNext= */ nextptr,
            /* flags= */ no_flag_bits,
            /* setLayoutCount= */ p_shader_info->descriptor_set_layout_count,
            /* pSetLayouts= */ p_shader_info->p_descriptor_set_layouts,
            /* pushConstantRangeCount= */ p_shader_info->push_const_count,
            /* pPushConstantRanges= */ p_shader_info->p_push_const_range,
    };
    vkCreatePipelineLayout(vk_device, &pipeline_layout_create_info, allocation_callbacks, &pipeline->layout);

    VkShaderModule compute_shader_module;
    compute_shader_module = load_shader_module(vk_device, p_shader_info->compute);

    VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
    shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shader_stage_create_info.module = compute_shader_module;
    shader_stage_create_info.pName = "main";

    VkComputePipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_create_info.stage = shader_stage_create_info;
    pipeline_create_info.layout = pipeline->layout;

    vkCreateComputePipelines(vk_device, VK_NULL_HANDLE, 1, &pipeline_create_info, VK_NULL_HANDLE, &pipeline->pipeline);
    vkDestroyShaderModule(vk_device, compute_shader_module, allocation_callbacks);

    return pipeline;
}

void RenderDevice::destroy_pipeline(Pipeline *p_pipeline)
{
    vkDestroyPipelineLayout(vk_device, p_pipeline->layout, allocation_callbacks);
    vkDestroyPipeline(vk_device, p_pipeline->pipeline, allocation_callbacks);
}

void RenderDevice::cmd_buffer_begin(VkCommandBuffer cmd_buffer, VkCommandBufferUsageFlags usage)
{
    VkCommandBufferBeginInfo cmd_buffer_begin_info = {
            /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            /* pNext */ nextptr,
            /* flags */ usage,
            /* pInheritanceInfo */ nullptr,
    };
    vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_begin_info);
}

void RenderDevice::cmd_buffer_end(VkCommandBuffer cmd_buffer)
{
    vkEndCommandBuffer(cmd_buffer);
}

void RenderDevice::cmd_buffer_one_time_begin(VkCommandBuffer *p_cmd_buffer)
{
    VkCommandBuffer cmd_buffer;
    allocate_cmd_buffer(&cmd_buffer);
    cmd_buffer_begin(cmd_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    *p_cmd_buffer = cmd_buffer;
}

void RenderDevice::cmd_buffer_one_time_end(VkCommandBuffer cmd_buffer)
{
    cmd_buffer_end(cmd_buffer);

    VkQueue graph_queue = vk_rdc->get_graph_queue();
    cmd_buffer_submit(cmd_buffer,
        0, VK_NULL_HANDLE,
        0, VK_NULL_HANDLE,
        VK_NULL_HANDLE,
        graph_queue,
        VK_NULL_HANDLE);
    vkQueueWaitIdle(graph_queue);

    free_cmd_buffer(cmd_buffer);
}

void RenderDevice::cmd_begin_render_pass(VkCommandBuffer cmd_buffer, VkRenderPass render_pass, uint32_t clear_value_count, VkClearValue *p_clear_values, VkFramebuffer framebuffer, VkRect2D *p_rect)
{
    VkRenderPassBeginInfo render_pass_begin_info = {
            /* sType */ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            /* pNext */ nextptr,
            /* renderPass */ render_pass,
            /* framebuffer */ framebuffer,
            /* renderArea */ *p_rect,
            /* clearValueCount */ clear_value_count,
            /* pClearValues */ p_clear_values,
    };

    vkCmdBeginRenderPass(cmd_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderDevice::cmd_pipeline_barrier(VkCommandBuffer cmd_buffer, const PipelineMemoryBarrier *p_pipeline_memory_barrier)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = p_pipeline_memory_barrier->image.src_access_mask;
    barrier.dstAccessMask = p_pipeline_memory_barrier->image.dst_access_mask;
    barrier.oldLayout = p_pipeline_memory_barrier->image.old_image_layout;
    barrier.newLayout = p_pipeline_memory_barrier->image.new_image_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = p_pipeline_memory_barrier->image.texture->image;
    barrier.subresourceRange.aspectMask = p_pipeline_memory_barrier->image.texture->aspect_mask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(
            cmd_buffer,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0,
            0, VK_NULL_HANDLE,
            0, VK_NULL_HANDLE,
            1, &barrier
    );

    p_pipeline_memory_barrier->image.texture->image_layout = barrier.newLayout;
}

void RenderDevice::cmd_end_render_pass(VkCommandBuffer cmd_buffer)
{
    vkCmdEndRenderPass(cmd_buffer);
}

void RenderDevice::cmd_bind_vertex_buffer(VkCommandBuffer cmd_buffer, RenderDevice::Buffer *p_buffer)
{
    VkBuffer buffers[] = { p_buffer->vk_buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd_buffer, 0, ARRAY_SIZE(buffers), buffers, offsets);
}

void RenderDevice::cmd_bind_index_buffer(VkCommandBuffer cmd_buffer, VkIndexType type, RenderDevice::Buffer *p_buffer)
{
    vkCmdBindIndexBuffer(cmd_buffer, p_buffer->vk_buffer, 0, type);
}

void RenderDevice::cmd_draw(VkCommandBuffer cmd_buffer, uint32_t vertex_count)
{
    vkCmdDraw(cmd_buffer, vertex_count, 1, 0, 0);
}

void RenderDevice::cmd_draw_indexed(VkCommandBuffer cmd_buffer, uint32_t index_count)
{
    vkCmdDrawIndexed(cmd_buffer, index_count, 1, 0, 0, 0);
}

void RenderDevice::cmd_bind_pipeline(VkCommandBuffer cmd_buffer, Pipeline *p_pipeline)
{
    vkCmdBindPipeline(cmd_buffer, p_pipeline->bind_point, p_pipeline->pipeline);
}

void RenderDevice::cmd_buffer_submit(VkCommandBuffer cmd_buffer, uint32_t wait_semaphore_count, VkSemaphore *p_wait_semaphore, uint32_t signal_semaphore_count, VkSemaphore *p_signal_semaphore, VkPipelineStageFlags *p_mask, VkQueue queue, VkFence fence)
{
    VkResult U_ASSERT_ONLY err;

    uint32_t cmd_buffer_count;
    VkCommandBuffer cmd_buffers[] = { cmd_buffer };
    cmd_buffer_count = cmd_buffer ? ARRAY_SIZE(cmd_buffers) : 0;

    VkSubmitInfo submit_info = {
            /* sType */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
            /* pNext */ nextptr,
            /* waitSemaphoreCount */ wait_semaphore_count,
            /* pWaitSemaphores */ p_wait_semaphore,
            /* pWaitDstStageMask */ p_mask,
            /* commandBufferCount */ cmd_buffer_count,
            /* pCommandBuffers */ cmd_buffers,
            /* signalSemaphoreCount */ signal_semaphore_count,
            /* pSignalSemaphores */ p_signal_semaphore,
    };

    err = vkQueueSubmit(queue, 1, &submit_info, fence);
    assert(!err);
}

void RenderDevice::cmd_bind_descriptor_set(VkCommandBuffer cmd_buffer, Pipeline *p_pipeline, VkDescriptorSet descriptor)
{
    vkCmdBindDescriptorSets(cmd_buffer, p_pipeline->bind_point, p_pipeline->layout, 0, 1, &descriptor, 0, VK_NULL_HANDLE);
}

void RenderDevice::cmd_setval_viewport(VkCommandBuffer cmd_buffer, uint32_t w, uint32_t h)
{
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = w;
    viewport.height = h;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { w, h };
    vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);
}

void RenderDevice::cmd_push_const(VkCommandBuffer cmd_buffer, RenderDevice::Pipeline *pipeline, VkShaderStageFlags shader_stage_flags, uint32_t offset, uint32_t size, void *p_values)
{
    vkCmdPushConstants(cmd_buffer, pipeline->layout, shader_stage_flags, offset, size, p_values);
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