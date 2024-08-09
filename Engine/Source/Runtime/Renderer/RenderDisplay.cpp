/* ======================================================================== */
/* RenderDisplay.cpp                                                        */
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
#include "RenderDisplay.h"
#include <algorithm>

RenderDisplay::RenderDisplay(RenderDevice *vRD)
    : rd(vRD)
{
    RenderDeviceContext* rdc = rd->GetDeviceContext();

    instance = rdc->GetInstance();
    physicalDevice = rdc->GetPhysicalDevice();
    device = rdc->GetDevice();
    graphQueueFamily = rdc->GetQueueFamily();
    cmdPool = rdc->GetCommandPool();
    graphQueue = rdc->GetQueue();
}

RenderDisplay::~RenderDisplay()
{
    vkDestroySemaphore(device, display->imageAvailableSemaphore, VK_NULL_HANDLE);
    vkDestroySemaphore(device, display->renderFinishedSemaphore, VK_NULL_HANDLE);
    vkDestroySwapchainKHR(device, display->swapchain, VK_NULL_HANDLE);
    vkDestroyRenderPass(device, display->renderPass, VK_NULL_HANDLE);
    _CleanUpSwapchain();
    vkDestroySurfaceKHR(instance, display->surface, VK_NULL_HANDLE);
    free(display);
}

void RenderDisplay::Initialize(Window *vCurrentNativeWindow)
{
    VkResult U_ASSERT_ONLY err;

    /* imalloc display struct and set surface */
    display = (Display *) imalloc(sizeof(Display));
    currentNativeWindow = vCurrentNativeWindow;
    currentNativeWindow->CreateWindowSurfaceKHR(instance, VK_NULL_HANDLE, &display->surface);

    VkSurfaceCapabilitiesKHR capabilities;
    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, display->surface, &capabilities);
    assert(!err);

    /* pick surface format */
    uint32_t format_count = 0;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, display->surface, &format_count, nullptr);
    assert(!err);

    VkSurfaceFormatKHR *surface_formats_khr = (VkSurfaceFormatKHR *) imalloc(sizeof(VkSurfaceFormatKHR) * format_count);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, display->surface, &format_count, surface_formats_khr);
    assert(!err);

    VkSurfaceFormatKHR surface_format = pick_surface_format(surface_formats_khr, format_count);
    display->format = surface_format.format;
    display->colorSpace = surface_format.colorSpace;

    free(surface_formats_khr);

    /* image buffer count */
    uint32_t desired_buffer_count = 3;
    desired_buffer_count = std::clamp(desired_buffer_count, capabilities.minImageCount, capabilities.maxImageCount);
    display->imageBufferCount = desired_buffer_count;

    display->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    display->presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    err = vkCreateSemaphore(device, &semaphore_create_info, VK_NULL_HANDLE, &display->imageAvailableSemaphore);
    assert(!err);

    err = vkCreateSemaphore(device, &semaphore_create_info, VK_NULL_HANDLE, &display->renderFinishedSemaphore);
    assert(!err);

    _CreateSwapchain();
}

void RenderDisplay::CmdBeginScreenRender(VkCommandBuffer *pCmdBuffer)
{
    _CheckUpdateSwapchain();
    vkAcquireNextImageKHR(device, display->swapchain, UINT64_MAX, display->imageAvailableSemaphore, nullptr, &acquireNextIndex);

    VkCommandBuffer cmdBuffer;
    cmdBuffer = display->swapchainResources[acquireNextIndex].cmdBuffer;
    *pCmdBuffer = cmdBuffer;
    rd->CmdBufferBegin(cmdBuffer, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

    VkClearValue clearColor = { 0.10f, 0.10f, 0.10f, 1.0f };

    VkRect2D rect = {};
    rect.extent = { display->width, display->height };
    rd->CmdBeginRenderPass(cmdBuffer, display->renderPass, 1, &clearColor, display->swapchainResources[acquireNextIndex].framebuffer, &rect);
}

void RenderDisplay::CmdEndScreenRender(VkCommandBuffer cmd_buffer)
{
    rd->CmdEndRenderPass(cmd_buffer);
    rd->CmdBufferEnd(cmd_buffer);

    VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    rd->CmdBufferSubmit(cmd_buffer, 1, &display->imageAvailableSemaphore, 1, &display->renderFinishedSemaphore, &mask, graphQueue, VK_NULL_HANDLE);
    rd->Present(graphQueue, display->swapchain, acquireNextIndex, display->renderFinishedSemaphore);
}

void RenderDisplay::_CreateSwapchain()
{
    VkResult U_ASSERT_ONLY err;
    VkSwapchainKHR oldSwapchain;

    oldSwapchain = display->swapchain;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, display->surface, &capabilities);
    display->width = capabilities.currentExtent.width;
    display->height = capabilities.currentExtent.height;

    if (!oldSwapchain)
    {
        // attachment
        VkAttachmentDescription attachment = {
                /* flags */ VK_NONE_FLAGS,
                /* format */ display->format,
                /* samples */ VK_SAMPLE_COUNT_1_BIT,
                /* loadOp */VK_ATTACHMENT_LOAD_OP_CLEAR,
                /* storeOp */ VK_ATTACHMENT_STORE_OP_STORE,
                /* stencilLoadOp */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
                /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
                /* finalLayout */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference reference = {};
        reference.attachment = 0;
        reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &reference;

        VkSubpassDependency subpass_dependency = {};
        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.dstSubpass = 0;
        subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.srcAccessMask = 0;
        subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // create render pass
        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &subpass_dependency;

        err = vkCreateRenderPass(device, &render_pass_create_info, VK_NULL_HANDLE, &display->renderPass);
        assert(!err);
    }

    /* create swap chain */
    VkSwapchainCreateInfoKHR swap_chain_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* surface */ display->surface,
            /* minImageCount */ display->imageBufferCount,
            /* imageFormat */ display->format,
            /* imageColorSpace */ display->colorSpace,
            /* imageExtent */ capabilities.currentExtent,
            /* imageArrayLayers */ 1,
            /* imageUsage */ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            /* imageSharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 1,
            /* pQueueFamilyIndices */ &graphQueueFamily,
            /* preTransform */ capabilities.currentTransform,
            /* compositeAlpha */ display->compositeAlpha,
            /* presentMode */ display->presentMode,
            /* clipped */ VK_TRUE,
            /* oldSwapchain */ oldSwapchain,
    };

    err = vkCreateSwapchainKHR(device, &swap_chain_create_info, VK_NULL_HANDLE, &display->swapchain);
    assert(!err);

    if (oldSwapchain)
        vkDestroySwapchainKHR(device, oldSwapchain, VK_NULL_HANDLE);

    /* initialize swap chain resources */
    display->swapchainResources = (SwapchainResource *) imalloc(sizeof(SwapchainResource) * display->imageBufferCount);

    std::vector<VkImage> swap_chain_images;
    swap_chain_images.resize(display->imageBufferCount);
    err = vkGetSwapchainImagesKHR(device, display->swapchain, &display->imageBufferCount, std::data(swap_chain_images));
    assert(!err);

    for (uint32_t i = 0; i < display->imageBufferCount; i++) {
        display->swapchainResources[i].image = swap_chain_images[i];

        VkCommandBufferAllocateInfo cmd_allocate_info = {
                /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                /* pNext */ VK_NULL_HANDLE,
                /* commandPool */ cmdPool,
                /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                /* commandBufferCount */ 1
        };

        rd->AllocateCommandBuffer(&(display->swapchainResources[i].cmdBuffer));

        VkImageViewCreateInfo image_view_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                /* pNext */ VK_NULL_HANDLE,
                /* flags */ VK_NONE_FLAGS,
                /* image */ display->swapchainResources[i].image,
                /* viewType */ VK_IMAGE_VIEW_TYPE_2D,
                /* format */ display->format,
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

        err = vkCreateImageView(device, &image_view_create_info, VK_NULL_HANDLE, &(display->swapchainResources[i].imageView));
        assert(!err);

        VkImageView framebuffer_attachments[] = { display->swapchainResources[i].imageView };

        VkFramebufferCreateInfo framebuffer_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                /* pNext */ VK_NULL_HANDLE,
                /* flags */ VK_NONE_FLAGS,
                /* renderPass */ display->renderPass,
                /* attachmentCount */ ARRAY_SIZE(framebuffer_attachments),
                /* pAttachments */ framebuffer_attachments,
                /* width */ display->width,
                /* height */ display->height,
                /* layers */ 1,
        };

        err = vkCreateFramebuffer(device, &framebuffer_create_info, VK_NULL_HANDLE, &(display->swapchainResources[i].framebuffer));
        assert(!err);
    }
}

void RenderDisplay::_CleanUpSwapchain()
{
    for (uint32_t i = 0; i < display->imageBufferCount; i++) {
        vkFreeCommandBuffers(device, cmdPool, 1, &display->swapchainResources[i].cmdBuffer);
        vkDestroyFramebuffer(device, display->swapchainResources[i].framebuffer, VK_NULL_HANDLE);
        vkDestroyImageView(device, display->swapchainResources[i].imageView, VK_NULL_HANDLE);
    }

    free(display->swapchainResources);
}

void RenderDisplay::_CheckUpdateSwapchain()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, display->surface, &capabilities);
    VkExtent2D extent = capabilities.currentExtent;

    /* is update */
    if ((extent.width != display->width || extent.height != display->height) && (extent.width != 0 || extent.height != 0)) {
        vkDeviceWaitIdle(device);
        _CleanUpSwapchain();
        _CreateSwapchain();
    }
}
