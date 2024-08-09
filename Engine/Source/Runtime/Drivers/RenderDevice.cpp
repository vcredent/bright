/* ======================================================================== */
/* render_device.cpp                                                        */
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
#include "RenderDevice.h"

RenderDevice::RenderDevice(RenderDeviceContext *vRDC)
    : rdc(vRDC)
{
    device = rdc->GetDevice();
    allocator = rdc->GetAllocator();

    _InitializeDescriptorPool();

    msaaSampleCounts = rdc->GetMaxMSAASampleCounts();

    // if sample counts > 4x，that default msaa samples set 4x otherwise 2x
    msaaSampleCounts = msaaSampleCounts >= 4 ? VK_SAMPLE_COUNT_4_BIT : VK_SAMPLE_COUNT_2_BIT;
}

RenderDevice::~RenderDevice()
{
    vkDestroyDescriptorPool(device, descriptorPool, VK_NULL_HANDLE);
}

RenderDevice::Buffer *RenderDevice::CreateBuffer(VkBufferUsageFlags usage, VkDeviceSize size)
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

    err = vmaCreateBuffer(allocator, &buffer_create_info, &allocation_create_info, &buffer->vkBuffer, &buffer->allocation, &buffer->allocationInfo);
    assert(!err);

    return buffer;
}

void RenderDevice::DestroyBuffer(Buffer *buffer)
{
    vmaDestroyBuffer(allocator, buffer->vkBuffer, buffer->allocation);
    free(buffer);
}

void RenderDevice::WriteBuffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf)
{
    char *tmp;
    vmaMapMemory(allocator, buffer->allocation, (void **) &tmp);
    memcpy((tmp + offset), buf, size);
    vmaUnmapMemory(allocator, buffer->allocation);
}

void
RenderDevice::ReadBuffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf)
{
    char *tmp;
    vmaMapMemory(allocator, buffer->allocation, (void **) &tmp);
    memcpy(buf, (tmp + offset), size);
    vmaUnmapMemory(allocator, buffer->allocation);
}

void RenderDevice::CreateRenderPass(uint32_t attachmentCount, VkAttachmentDescription *pAttachments, uint32_t subpassCount, VkSubpassDescription *pSubpass, uint32_t dependencyCount, VkSubpassDependency *pDependencies, VkRenderPass *pRenderPass)
{
    VkResult U_ASSERT_ONLY err;

    // create render pass
    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = attachmentCount;
    render_pass_create_info.pAttachments = pAttachments;
    render_pass_create_info.subpassCount = subpassCount;
    render_pass_create_info.pSubpasses = pSubpass;
    render_pass_create_info.dependencyCount = dependencyCount;
    render_pass_create_info.pDependencies = pDependencies;

    err = vkCreateRenderPass(device, &render_pass_create_info, VK_NULL_HANDLE, pRenderPass);
    assert(!err);
}

void RenderDevice::DestroyRenderPass(VkRenderPass renderPass)
{
    vkDestroyRenderPass(device, renderPass, VK_NULL_HANDLE);
}

void RenderDevice::AllocateCommandBuffer(VkCommandBuffer *pCmdBuffer)
{
    rdc->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCmdBuffer);
}

void RenderDevice::FreeCommandBuffer(VkCommandBuffer cmdBuffer)
{
    rdc->FreeCommandBuffer(cmdBuffer);
}

RenderDevice::Texture2D *RenderDevice::CreateTexture(TextureCreateInfo *pCreateInfo)
{
    VkResult U_ASSERT_ONLY err;
    Texture2D *texture = VK_NULL_HANDLE;

    texture = (Texture2D *) imalloc(sizeof(Texture2D));
    texture->format = pCreateInfo->format;
    texture->width = pCreateInfo->width;
    texture->height = pCreateInfo->height;
    texture->aspectMask = pCreateInfo->aspectMask;

    VkImageCreateInfo image_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* imageType */ pCreateInfo->imageType,
            /* format */ texture->format,
            /* extent */ { pCreateInfo->width, pCreateInfo->height, 1 },
            /* mipLevels */ 1,
            /* arrayLayers */ 1,
            /* samples */ pCreateInfo->samples,
            /* tiling */ VK_IMAGE_TILING_OPTIMAL,
            /* usage */ pCreateInfo->usage,
            /* sharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 0,
            /* pQueueFamilyIndices */ nullptr,
            /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    err = vmaCreateImage(allocator, &image_create_info, &allocation_create_info, &texture->image, &texture->allocation, &texture->allocationInfo);
    assert(!err);

    VkImageViewCreateInfo image_view_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* image */ texture->image,
            /* viewType */ pCreateInfo->imageViewType,
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
                    .aspectMask = pCreateInfo->aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
    };

    err = vkCreateImageView(device, &image_view_create_info, VK_NULL_HANDLE, &texture->imageView);
    assert(!err);

    return texture;
}

void RenderDevice::DestroyTexture(Texture2D *p_texture)
{
    vmaDestroyImage(allocator, p_texture->image, p_texture->allocation);
    vkDestroyImageView(device, p_texture->imageView, VK_NULL_HANDLE);
    if (p_texture->descriptorSet)
        FreeDescriptorSet(p_texture->descriptorSet);
}

void RenderDevice::WriteTexture(Texture2D *texture, size_t size, void *pixels)
{
    Buffer *buffer = CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size);
    WriteBuffer(buffer, 0, size, pixels);

    texture->size = size;

    VkCommandBuffer cmdBuffer;
    CmdBufferOneTimeBegin(&cmdBuffer);

    PipelineMemoryBarrier barrier;
    barrier.image.texture = texture;
    barrier.image.oldImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.image.newImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.image.srcAccessMask = 0;
    barrier.image.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    CmdPipelineBarrier(cmdBuffer, &barrier);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = { texture->width, texture->height, 1 };

    vkCmdCopyBufferToImage(
        cmdBuffer,
        buffer->vkBuffer,
        texture->image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    barrier.image.oldImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.image.newImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.image.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.image.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    CmdPipelineBarrier(cmdBuffer, &barrier);

    CmdBufferOneTimeEnd(cmdBuffer);
    DestroyBuffer(buffer);
}

void
RenderDevice::CreateFramebuffer(uint32_t width, uint32_t height, uint32_t image_view_count, VkImageView *p_image_view, VkRenderPass renderPass, VkFramebuffer *p_framebuffer)
{
    VkResult U_ASSERT_ONLY err;

    VkFramebufferCreateInfo framebuffer_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* renderPass */ renderPass,
            /* attachmentCount */ image_view_count,
            /* pAttachments */ p_image_view,
            /* width */ width,
            /* height */ height,
            /* layers */ 1,
    };

    err = vkCreateFramebuffer(device, &framebuffer_create_info, VK_NULL_HANDLE, p_framebuffer);
    assert(!err);
}

void RenderDevice::DestroyFramebuffer(VkFramebuffer framebuffer)
{
    vkDestroyFramebuffer(device, framebuffer, VK_NULL_HANDLE);
}

void RenderDevice::CreateSampler(SamplerCreateInfo* pCreateInfo, VkSampler* p_sampler)
{
    VkSamplerCreateInfo sampler_create_info = {};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.addressModeU = pCreateInfo->u;
    sampler_create_info.addressModeV = pCreateInfo->v;
    sampler_create_info.addressModeW = pCreateInfo->w;
    sampler_create_info.anisotropyEnable = VK_FALSE;
    sampler_create_info.maxAnisotropy = 16;
    sampler_create_info.borderColor = pCreateInfo->border_color;
    sampler_create_info.unnormalizedCoordinates = VK_FALSE;
    sampler_create_info.compareEnable = VK_FALSE;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.minLod = 0.0f;
    sampler_create_info.maxLod = 0.0f;

    vkCreateSampler(device, &sampler_create_info, VK_NULL_HANDLE, p_sampler);
}

void RenderDevice::DestroySampler(VkSampler sampler)
{
    vkDestroySampler(device, sampler, VK_NULL_HANDLE);
}

void RenderDevice::BindTextureSampler(RenderDevice::Texture2D *texture, VkSampler sampler)
{
    texture->sampler = sampler;
}

void RenderDevice::CreateDescriptorSetLayout(uint32_t bindingCount, VkDescriptorSetLayoutBinding *pBindings, VkDescriptorSetLayout *pDescriptorSetLayout)
{
    VkResult U_ASSERT_ONLY err;

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* bindingCount */ bindingCount,
            /* pBindings */ pBindings,
    };

    err = vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info, VK_NULL_HANDLE, pDescriptorSetLayout);
    assert(!err);
}

void RenderDevice::DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
{
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, VK_NULL_HANDLE);
}

void RenderDevice::AllocateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet *pDescriptorSet)
{
    VkDescriptorSetAllocateInfo descriptor_allocate_info = {
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* descriptorPool */ descriptorPool,
            /* descriptorSetCount */ 1,
            /* pSetLayouts */ &descriptorSetLayout,
    };

    vkAllocateDescriptorSets(device, &descriptor_allocate_info, pDescriptorSet);
}

void RenderDevice::FreeDescriptorSet(VkDescriptorSet descriptorSet)
{
    vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
}

void RenderDevice::UpdateDescriptorSetBuffer(Buffer *buffer, uint32_t binding, VkDescriptorSet descriptorSet)
{
    VkDescriptorBufferInfo bufferInfo = {
            /* buffer */ buffer->vkBuffer,
            /* offset */ 0,
            /* range */ buffer->size,
    };

    VkWriteDescriptorSet writeInfo = {
            /* sType */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /* pNext */ VK_NULL_HANDLE,
            /* dstSet */ descriptorSet,
            /* dstBinding */ binding,
            /* dstArrayElement */ 0,
            /* descriptorCount */ 1,
            /* descriptorType */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /* pImageInfo */ VK_NULL_HANDLE,
            /* pBufferInfo */ &bufferInfo,
            /* pTexelBufferView */ VK_NULL_HANDLE,
    };

    vkUpdateDescriptorSets(device, 1, &writeInfo, 0, nullptr);
}

void RenderDevice::UpdateDescriptorSetImage(RenderDevice::Texture2D *texture, uint32_t binding, VkDescriptorSet descriptorSet)
{
    VkDescriptorImageInfo image_info = {
            /* sampler= */ texture->sampler,
            /* imageView= */ texture->imageView,
            /* imageLayout= */ texture->imageLayout,
    };

    VkWriteDescriptorSet writeInfo = {
            /* sType */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /* pNext */ VK_NULL_HANDLE,
            /* dstSet */ descriptorSet,
            /* dstBinding */ binding,
            /* dstArrayElement */ 0,
            /* descriptorCount */ 1,
            /* descriptorType */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            /* pImageInfo */ &image_info,
            /* pBufferInfo */ VK_NULL_HANDLE,
            /* pTexelBufferView */ VK_NULL_HANDLE,
    };

    vkUpdateDescriptorSets(device, 1, &writeInfo, 0, nullptr);
}

RenderDevice::Pipeline *RenderDevice::CreateGraphicsPipeline(RenderDevice::PipelineCreateInfo *pCreateInfo, RenderDevice::ShaderInfo *pShaderInfo)
{
    VkResult U_ASSERT_ONLY err;

    VkPipelineLayoutCreateInfo pipelineLayoutCreatInfo = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* setLayoutCount */ pShaderInfo->descriptorSetLayoutCount,
            /* pSetLayouts */ pShaderInfo->pDescriptorSetLayouts,
            /* pushConstantRangeCount */ pShaderInfo->pushConstantCount,
            /* pPushConstantRanges */ pShaderInfo->pPushConstantRange,
    };

    VkPipelineLayout pipelineLayout;
    err = vkCreatePipelineLayout(device, &pipelineLayoutCreatInfo, VK_NULL_HANDLE, &pipelineLayout);
    assert(!err);

    VkShaderModule vertex_shader_module, fragment_shader_module;

    vertex_shader_module = load_shader_module(device, pShaderInfo->vertex, "vert");
    fragment_shader_module = load_shader_module(device, pShaderInfo->fragment, "frag");

    VkPipelineShaderStageCreateInfo vertex_shader_create_info = {};
    vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_create_info.module = vertex_shader_module;
    vertex_shader_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
    fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderCreateInfo.module = fragment_shader_module;
    fragmentShaderCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStagesInfo[] = {
            vertex_shader_create_info, fragmentShaderCreateInfo
    };

    VkPipelineVertexInputStateCreateInfo inputStateCreateInfo = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* vertexBindingDescriptionCount */ pShaderInfo->bindCount,
            /* pVertexBindingDescriptions */ pShaderInfo->binds,
            /* vertexAttributeDescriptionCount */ pShaderInfo->attributeCount,
            /* pVertexAttributeDescriptions */ pShaderInfo->attributes,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* topology */ pCreateInfo->topology,
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

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* viewportCount */ 1,
            /* pViewports */ &viewport,
            /* scissorCount */ 1,
            /* pScissors */ &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = pCreateInfo->polygon;
    rasterizationStateCreateInfo.lineWidth = pCreateInfo->lineWidth;
    rasterizationStateCreateInfo.cullMode = pCreateInfo->cullMode;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = pCreateInfo->samples;
    multisampleStateCreateInfo.minSampleShading = 1.0f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = pCreateInfo->blendEnable;
    colorBlendAttachmentState.srcColorBlendFactor = pCreateInfo->srcColorBlendFactor;
    colorBlendAttachmentState.dstColorBlendFactor = pCreateInfo->dstColorBlendFactor;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.blendEnable = VK_TRUE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds = 0.0f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamics = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };

    if (pCreateInfo->lineWidth > 1.0f)
        dynamics.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);

    VkPipelineDynamicStateCreateInfo dynamicStateCrateInfo = {
            /* sType= */ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            /* pNext= */ VK_NULL_HANDLE,
            /* flags= */ VK_NONE_FLAGS,
            /* dynamicStateCount= */ (uint32_t) std::size(dynamics),
            /* pDynamicStates= */ std::data(dynamics),
    };

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
            /* sType */ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* stageCount */ ARRAY_SIZE(shaderStagesInfo),
            /* pStages */ shaderStagesInfo,
            /* pVertexInputState */ &inputStateCreateInfo,
            /* pInputAssemblyState */ &inputAssembly,
            /* pTessellationState */ nullptr,
            /* pViewportState */ &viewportStateCreateInfo,
            /* pRasterizationState */ &rasterizationStateCreateInfo,
            /* pMultisampleState */ &multisampleStateCreateInfo,
            /* pDepthStencilState */ &depthStencilStateCreateInfo,
            /* pColorBlendState */ &colorBlendStateCreateInfo,
            /* pDynamicState */ &dynamicStateCrateInfo,
            /* layout */ pipelineLayout,
            /* renderPass */ pCreateInfo->renderPass,
            /* subpass */ 0,
            /* basePipelineHandle */ VK_NULL_HANDLE,
            /* basePipelineIndex */ -1,
    };

    VkPipeline pipeline;
    err = vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineCreateInfo, VK_NULL_HANDLE, &pipeline);
    assert(!err);

    Pipeline *pPipeline = (Pipeline*) imalloc(sizeof(Pipeline));
    pPipeline->pipeline = pipeline;
    pPipeline->layout = pipelineLayout;
    pPipeline->bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    vkDestroyShaderModule(device, vertex_shader_module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, fragment_shader_module, VK_NULL_HANDLE);

    return pPipeline;
}

void RenderDevice::_InitializeDescriptorPool()
{
    VkResult U_ASSERT_ONLY err;

    VkDescriptorPoolSize poolSize[] = {
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

    VkDescriptorPoolCreateInfo descriptorPool_create_info = {
            /* STYPE */ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            /* PNEXT */ VK_NULL_HANDLE,
            /* FLAGS */ VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            /* MAXSETS */ 1024,
            /* POOLSIZECOUNT */ ARRAY_SIZE(poolSize),
            /* PPOOLSIZES */ poolSize,
    };

    err = vkCreateDescriptorPool(device, &descriptorPool_create_info, VK_NULL_HANDLE, &descriptorPool);
    assert(!err);
}

RenderDevice::Pipeline *RenderDevice::CreateComputePipeline(RenderDevice::ComputeShaderInfo *pShaderInfo)
{
    Pipeline *pipeline = (Pipeline *) imalloc(sizeof(Pipeline));
    pipeline->bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
            /* sType= */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            /* pNext= */ VK_NULL_HANDLE,
            /* flags= */ VK_NONE_FLAGS,
            /* setLayoutCount= */ pShaderInfo->descriptorSetLayoutCount,
            /* pSetLayouts= */ pShaderInfo->pDescriptorSetLayouts,
            /* pushConstantRangeCount= */ pShaderInfo->pushConstantCount,
            /* pPushConstantRanges= */ pShaderInfo->pPushConstantRange,
    };
    vkCreatePipelineLayout(device, &pipeline_layout_create_info, VK_NULL_HANDLE, &pipeline->layout);

    VkShaderModule compute_shader_module;
    compute_shader_module = load_shader_module(device, pShaderInfo->compute, "vert");

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = compute_shader_module;
    shaderStageCreateInfo.pName = "main";

    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = shaderStageCreateInfo;
    pipelineCreateInfo.layout = pipeline->layout;

    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, VK_NULL_HANDLE, &pipeline->pipeline);
    vkDestroyShaderModule(device, compute_shader_module, VK_NULL_HANDLE);

    return pipeline;
}

void RenderDevice::DestroyPipeline(RenderDevice::Pipeline *pPipeline)
{
    vkDestroyPipelineLayout(device, pPipeline->layout, VK_NULL_HANDLE);
    vkDestroyPipeline(device, pPipeline->pipeline, VK_NULL_HANDLE);
}

void RenderDevice::CmdBufferBegin(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usage)
{
    VkCommandBufferBeginInfo cmdBufferBeginInfo = {
            /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ usage,
            /* pInheritanceInfo */ nullptr,
    };
    vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
}

void RenderDevice::CmdBufferEnd(VkCommandBuffer cmdBuffer)
{
    vkEndCommandBuffer(cmdBuffer);
}

void RenderDevice::CmdBufferOneTimeBegin(VkCommandBuffer *pCmdBuffer)
{
    VkCommandBuffer cmdBuffer;
    AllocateCommandBuffer(&cmdBuffer);
    CmdBufferBegin(cmdBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    *pCmdBuffer = cmdBuffer;
}

void RenderDevice::CmdBufferOneTimeEnd(VkCommandBuffer cmdBuffer)
{
    CmdBufferEnd(cmdBuffer);

    VkQueue graph_queue = rdc->GetQueue();
    CmdBufferSubmit(cmdBuffer,
        0, VK_NULL_HANDLE,
        0, VK_NULL_HANDLE,
        VK_NULL_HANDLE,
        graph_queue,
        VK_NULL_HANDLE);
    vkQueueWaitIdle(graph_queue);

    FreeCommandBuffer(cmdBuffer);
}

void RenderDevice::CmdBeginRenderPass(VkCommandBuffer cmdBuffer, VkRenderPass renderPass, uint32_t clearValueCount, VkClearValue *pClearValues, VkFramebuffer framebuffer, VkRect2D *pRect2D)
{
    VkRenderPassBeginInfo render_pass_begin_info = {
            /* sType */ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* renderPass */ renderPass,
            /* framebuffer */ framebuffer,
            /* renderArea */ *pRect2D,
            /* clearValueCount */ clearValueCount,
            /* pClearValues */ pClearValues,
    };

    vkCmdBeginRenderPass(cmdBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderDevice::CmdPipelineBarrier(VkCommandBuffer cmdBuffer, const RenderDevice::PipelineMemoryBarrier *pPipelineMemoryBarrier)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = pPipelineMemoryBarrier->image.srcAccessMask;
    barrier.dstAccessMask = pPipelineMemoryBarrier->image.dstAccessMask;
    barrier.oldLayout = pPipelineMemoryBarrier->image.oldImageLayout;
    barrier.newLayout = pPipelineMemoryBarrier->image.newImageLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = pPipelineMemoryBarrier->image.texture->image;
    barrier.subresourceRange.aspectMask = pPipelineMemoryBarrier->image.texture->aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(
            cmdBuffer,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0,
            0, VK_NULL_HANDLE,
            0, VK_NULL_HANDLE,
            1, &barrier
    );

    pPipelineMemoryBarrier->image.texture->imageLayout = barrier.newLayout;
}

void RenderDevice::CmdEndRenderPass(VkCommandBuffer cmdBuffer)
{
    vkCmdEndRenderPass(cmdBuffer);
}

void RenderDevice::CmdBindVertexBuffer(VkCommandBuffer cmdBuffer, RenderDevice::Buffer *buffer)
{
    VkBuffer buffers[] = { buffer->vkBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, ARRAY_SIZE(buffers), buffers, offsets);
}

void RenderDevice::CmdBindIndexBuffer(VkCommandBuffer cmdBuffer, VkIndexType type, RenderDevice::Buffer *buffer)
{
    vkCmdBindIndexBuffer(cmdBuffer, buffer->vkBuffer, 0, type);
}

void RenderDevice::CmdDraw(VkCommandBuffer cmdBuffer, uint32_t vertexCount)
{
    vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);
}

void RenderDevice::CmdDrawIndexed(VkCommandBuffer cmdBuffer, uint32_t indexCount)
{
    vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0);
}

void RenderDevice::CmdBindPipeline(VkCommandBuffer cmdBuffer, RenderDevice::Pipeline *pPipeline)
{
    vkCmdBindPipeline(cmdBuffer, pPipeline->bindPoint, pPipeline->pipeline);
}

void RenderDevice::CmdBufferSubmit(VkCommandBuffer cmdBuffer, uint32_t waitSemaphoreCount, VkSemaphore *pWaitSemaphores, uint32_t signalSemaphoreCount, VkSemaphore *pSignalSemaphores, VkPipelineStageFlags *pMask, VkQueue queue, VkFence fence)
{
    VkResult U_ASSERT_ONLY err;

    uint32_t cmd_buffer_count;
    VkCommandBuffer cmd_buffers[] = { cmdBuffer };
    cmd_buffer_count = cmdBuffer ? ARRAY_SIZE(cmd_buffers) : 0;

    VkSubmitInfo submit_info = {
            /* sType */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* waitSemaphoreCount */ waitSemaphoreCount,
            /* pWaitSemaphores */ pWaitSemaphores,
            /* pWaitDstStageMask */ pMask,
            /* commandBufferCount */ cmd_buffer_count,
            /* pCommandBuffers */ cmd_buffers,
            /* signalSemaphoreCount */ signalSemaphoreCount,
            /* pSignalSemaphores */ pSignalSemaphores,
    };

    err = vkQueueSubmit(queue, 1, &submit_info, fence);
    assert(!err);
}

void RenderDevice::CmdBindDescriptorSet(VkCommandBuffer cmdBuffer, RenderDevice::Pipeline *pPipeline, VkDescriptorSet descriptor)
{
    vkCmdBindDescriptorSets(cmdBuffer, pPipeline->bindPoint, pPipeline->layout, 0, 1, &descriptor, 0, VK_NULL_HANDLE);
}

void RenderDevice::CmdSetViewport(VkCommandBuffer cmdBuffer, uint32_t w, uint32_t h)
{
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = w;
    viewport.height = h;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { w, h };
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
}

void RenderDevice::CmdPushConstant(VkCommandBuffer cmdBuffer, RenderDevice::Pipeline *pipeline, VkShaderStageFlags shaderStageFlags, uint32_t offset, uint32_t size, void *pValues)
{
    vkCmdPushConstants(cmdBuffer, pipeline->layout, shaderStageFlags, offset, size, pValues);
}

void RenderDevice::Present(VkQueue queue, VkSwapchainKHR swapchain, uint32_t index, VkSemaphore waitSemaphore)
{
    VkPresentInfoKHR present_info = {
            /* sType */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            /* pNext */ VK_NULL_HANDLE,
            /* waitSemaphoreCount */ 1,
            /* pWaitSemaphores */ &waitSemaphore,
            /* swapchainCount */ 1,
            /* pSwapchains */ &swapchain,
            /* pImageIndices */ &index,
            /* pResults */ VK_NULL_HANDLE,
    };

    vkQueuePresentKHR(queue, &present_info);
    vkQueueWaitIdle(queue);
}