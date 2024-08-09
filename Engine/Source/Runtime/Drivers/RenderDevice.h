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

#include "RenderDeviceContext.h"
#include <vector>

class RenderDevice {
public:
    RenderDevice(RenderDeviceContext *vRDC);
    ~RenderDevice();

    RenderDeviceContext *GetDeviceContext() { return rdc; }
    VkDescriptorPool GetDescriptorPool() { return descriptorPool; }
    VkFormat GetSurfaceFormat() { return rdc->GetWindowFormat(); }
    VkSampleCountFlagBits GetMSAASampleCounts() { return msaaSampleCounts; }

    struct Buffer {
        VkBuffer vkBuffer;
        VkDeviceSize size;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;
    };

    Buffer *CreateBuffer(VkBufferUsageFlags usage, VkDeviceSize size);
    void DestroyBuffer(Buffer *buffer);
    void WriteBuffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);
    void ReadBuffer(Buffer *buffer, VkDeviceSize offset, VkDeviceSize size, void *buf);

    void CreateRenderPass(uint32_t attachmentCount, VkAttachmentDescription *pAttachments, uint32_t subpassCount, VkSubpassDescription *pSubpass, uint32_t dependencyCount, VkSubpassDependency *pDependencies, VkRenderPass *pRenderPass);
    void DestroyRenderPass(VkRenderPass renderPass);
    void AllocateCommandBuffer(VkCommandBuffer *pCmdBuffer);
    void FreeCommandBuffer(VkCommandBuffer cmdBuffer);

    struct Texture2D {
        VkImage image;
        VkImageView imageView;
        VkImageLayout imageLayout;
        VkDescriptorSet descriptorSet;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;
        uint32_t width;
        uint32_t height;
        VkFormat format;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageAspectFlags aspectMask;
        size_t size = 0;
    };

    struct TextureCreateInfo {
        uint32_t width;
        uint32_t height;
        VkSampleCountFlagBits samples;
        VkFormat format;
        VkImageAspectFlags aspectMask;
        VkImageType imageType;
        VkImageViewType imageViewType;
        VkImageUsageFlags usage;
    };

    Texture2D *CreateTexture(TextureCreateInfo *pCreateInfo);
    void DestroyTexture(Texture2D *p_texture);
    void WriteTexture(Texture2D *texture, size_t size, void *pixels);
    void CreateFramebuffer(uint32_t width, uint32_t height, uint32_t image_view_count, VkImageView *p_image_view, VkRenderPass renderPass, VkFramebuffer *p_framebuffer);
    void DestroyFramebuffer(VkFramebuffer framebuffer);

    struct SamplerCreateInfo {
        VkSamplerAddressMode u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode w = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkBorderColor border_color = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    };

    void CreateSampler(SamplerCreateInfo* pCreateInfo, VkSampler *p_sampler);
    void DestroySampler(VkSampler sampler);
    void BindTextureSampler(Texture2D *texture, VkSampler sampler);

    void CreateDescriptorSetLayout(uint32_t bindingCount, VkDescriptorSetLayoutBinding *pBindings, VkDescriptorSetLayout *pDescriptorSetLayout);
    void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);
    void AllocateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet *pDescriptorSet);
    void FreeDescriptorSet(VkDescriptorSet descriptorSet);
    void UpdateDescriptorSetBuffer(Buffer *buffer, uint32_t binding, VkDescriptorSet descriptorSet);
    void UpdateDescriptorSetImage(Texture2D *texture, uint32_t binding, VkDescriptorSet descriptorSet);

    struct ShaderInfo {
        const char *vertex = NULL;
        const char *fragment = NULL;
        uint32_t attributeCount = 0;
        VkVertexInputAttributeDescription *attributes = NULL;
        uint32_t bindCount = 0;
        VkVertexInputBindingDescription *binds = NULL;
        uint32_t descriptorSetLayoutCount = 0;
        VkDescriptorSetLayout *pDescriptorSetLayouts = NULL;
        uint32_t pushConstantCount = 0;
        VkPushConstantRange *pPushConstantRange = NULL;
    };

    struct ComputeShaderInfo {
        const char *compute = NULL;
        uint32_t descriptorSetLayoutCount = 0;
        VkDescriptorSetLayout *pDescriptorSetLayouts = NULL;
        uint32_t pushConstantCount = 0;
        VkPushConstantRange *pPushConstantRange = NULL;
    };

    struct PipelineCreateInfo {
        VkRenderPass renderPass;
        VkPolygonMode polygon;
        VkPrimitiveTopology topology;
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
        float lineWidth = 1.0f;
        VkBool32 blendEnable = VK_FALSE;
        VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    };

    struct Pipeline {
        VkPipeline pipeline;
        VkPipelineLayout layout;
        VkPipelineBindPoint bindPoint;
    };

    Pipeline *CreateGraphicsPipeline(PipelineCreateInfo *pCreateInfo, ShaderInfo *pShaderInfo);
    Pipeline *CreateComputePipeline(ComputeShaderInfo *pShaderInfo);
    void DestroyPipeline(Pipeline *pPipeline);

    void CmdBufferBegin(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usage);
    void CmdBufferEnd(VkCommandBuffer cmdBuffer);
    void CmdBufferOneTimeBegin(VkCommandBuffer *pCmdBuffer);
    void CmdBufferOneTimeEnd(VkCommandBuffer cmdBuffer);

    struct PipelineMemoryBarrier {
        struct {
            Texture2D *texture = VK_NULL_HANDLE;
            VkImageLayout oldImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkImageLayout newImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkAccessFlags srcAccessMask = 0;
            VkAccessFlags dstAccessMask = 0;
        } image;
    };

    void CmdPipelineBarrier(VkCommandBuffer cmdBuffer, const PipelineMemoryBarrier *pPipelineMemoryBarrier);

    void CmdBeginRenderPass(VkCommandBuffer cmdBuffer, VkRenderPass renderPass, uint32_t clearValueCount, VkClearValue *pClearValues, VkFramebuffer framebuffer, VkRect2D *pRect2D);
    void CmdEndRenderPass(VkCommandBuffer cmdBuffer);
    void CmdBindVertexBuffer(VkCommandBuffer cmdBuffer, Buffer *buffer);
    void CmdBindIndexBuffer(VkCommandBuffer cmdBuffer, VkIndexType type, Buffer *buffer);
    void CmdDraw(VkCommandBuffer cmdBuffer, uint32_t vertexCount);
    void CmdDrawIndexed(VkCommandBuffer cmdBuffer, uint32_t indexCount);
    void CmdBindPipeline(VkCommandBuffer cmdBuffer, Pipeline *pPipeline);
    void CmdBufferSubmit(VkCommandBuffer cmdBuffer, uint32_t waitSemaphoreCount, VkSemaphore *pWaitSemaphores, uint32_t signalSemaphoreCount, VkSemaphore *pSignalSemaphores, VkPipelineStageFlags *pMask, VkQueue queue, VkFence fence);
    void CmdBindDescriptorSet(VkCommandBuffer cmdBuffer, Pipeline *pPipeline, VkDescriptorSet descriptor);
    void CmdSetViewport(VkCommandBuffer cmdBuffer , uint32_t w, uint32_t h);
    void CmdPushConstant(VkCommandBuffer cmdBuffer, RenderDevice::Pipeline *pipeline, VkShaderStageFlags shaderStageFlags, uint32_t offset, uint32_t size, void *pValues);
    void Present(VkQueue queue, VkSwapchainKHR swapchain, uint32_t index, VkSemaphore waitSemaphore);

private:
    void _InitializeDescriptorPool();

    RenderDeviceContext *rdc;
    VkDevice device;
    VmaAllocator allocator;
    VkDescriptorPool descriptorPool;
    VkSampleCountFlagBits msaaSampleCounts;
};

#endif /* _RENDERING_DEVICE_DRIVER_VULKAN_H */