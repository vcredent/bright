/* ======================================================================== */
/* RenderScreen.cpp                                                         */
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
#include "RenderScreen.h"
#include <algorithm>

RenderScreen::RenderScreen(RenderDevice *vRD)
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

RenderScreen::~RenderScreen()
{
    vkDestroySemaphore(device, window->imageAvailableSemaphore, VK_NULL_HANDLE);
    vkDestroySemaphore(device, window->renderFinishedSemaphore, VK_NULL_HANDLE);
    vkDestroySwapchainKHR(device, window->swapchain, VK_NULL_HANDLE);
    vkDestroyRenderPass(device, window->renderPass, VK_NULL_HANDLE);
    _CleanUpSwapchain();
    vkDestroySurfaceKHR(instance, window->surface, VK_NULL_HANDLE);
    free(window);
}

void RenderScreen::Initialize(Window *vCurrentNativeWindow)
{
    VkResult U_ASSERT_ONLY err;

    /* imalloc display window struct and set surface */
    window = (_Window *) imalloc(sizeof(_Window));
    currentNativeWindow = vCurrentNativeWindow;
    currentNativeWindow->CreateWindowSurfaceKHR(instance, VK_NULL_HANDLE, &window->surface);

    VkSurfaceCapabilitiesKHR capabilities;
    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, window->surface, &capabilities);
    assert(!err);

    /* pick surface format */
    uint32_t format_count = 0;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, window->surface, &format_count, nullptr);
    assert(!err);

    VkSurfaceFormatKHR *surface_formats_khr = (VkSurfaceFormatKHR *) imalloc(sizeof(VkSurfaceFormatKHR) * format_count);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, window->surface, &format_count, surface_formats_khr);
    assert(!err);

    VkSurfaceFormatKHR surface_format = pick_surface_format(surface_formats_khr, format_count);
    window->format = surface_format.format;
    window->colorSpace = surface_format.colorSpace;

    free(surface_formats_khr);

    /* image buffer count */
    uint32_t desired_buffer_count = 3;
    desired_buffer_count = std::clamp(desired_buffer_count, capabilities.minImageCount, capabilities.maxImageCount);
    window->imageBufferCount = desired_buffer_count;

    window->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    window->presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    err = vkCreateSemaphore(device, &semaphore_create_info, VK_NULL_HANDLE, &window->imageAvailableSemaphore);
    assert(!err);

    err = vkCreateSemaphore(device, &semaphore_create_info, VK_NULL_HANDLE, &window->renderFinishedSemaphore);
    assert(!err);

    _CreateSwapchain();
}

void RenderScreen::CmdBeginScreenRender(VkCommandBuffer *pCmdBuffer)
{
    _UpdateSwapchain();
    vkAcquireNextImageKHR(device, window->swapchain, UINT64_MAX, window->imageAvailableSemaphore, nullptr, &acquireNextIndex);

    VkCommandBuffer cmdBuffer;
    cmdBuffer = window->swapchainResources[acquireNextIndex].cmdBuffer;
    *pCmdBuffer = cmdBuffer;
    rd->CmdBufferBegin(cmdBuffer, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

    VkClearValue clearColor = {
            0.10f, 0.10f, 0.10f, 1.0f
    };

    VkRect2D rect = {};
    rect.extent = { window->width, window->height };
    rd->CmdBeginRenderPass(cmdBuffer, window->renderPass, 1, &clearColor, window->swapchainResources[acquireNextIndex].framebuffer, &rect);
}

void RenderScreen::CmdEndScreenRender(VkCommandBuffer cmd_buffer)
{
    rd->CmdEndRenderPass(cmd_buffer);
    rd->CmdBufferEnd(cmd_buffer);

    VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    rd->CmdBufferSubmit(cmd_buffer, 1, &window->imageAvailableSemaphore, 1, &window->renderFinishedSemaphore, &mask, graphQueue, VK_NULL_HANDLE);
    rd->Present(graphQueue, window->swapchain, acquireNextIndex, window->renderFinishedSemaphore);
}

void RenderScreen::_CreateSwapchain()
{
    VkResult U_ASSERT_ONLY err;
    VkSwapchainKHR oldSwapchain;

    oldSwapchain = window->swapchain;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, window->surface, &capabilities);
    window->width = capabilities.currentExtent.width;
    window->height = capabilities.currentExtent.height;

    if (!oldSwapchain)
    {
        // attachment
        VkAttachmentDescription color_attachment = {
                /* flags */ VK_NONE_FLAGS,
                /* format */ window->format,
                /* samples */ VK_SAMPLE_COUNT_1_BIT,
                /* loadOp */VK_ATTACHMENT_LOAD_OP_CLEAR,
                /* storeOp */ VK_ATTACHMENT_STORE_OP_STORE,
                /* stencilLoadOp */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
                /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
                /* finalLayout */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference color_attachment_reference = {};
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_reference;

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
        render_pass_create_info.pAttachments = &color_attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &subpass_dependency;

        err = vkCreateRenderPass(device, &render_pass_create_info, VK_NULL_HANDLE, &window->renderPass);
        assert(!err);
    }

    /* create swap chain */
    VkSwapchainCreateInfoKHR swap_chain_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* surface */ window->surface,
            /* minImageCount */ window->imageBufferCount,
            /* imageFormat */ window->format,
            /* imageColorSpace */ window->colorSpace,
            /* imageExtent */ capabilities.currentExtent,
            /* imageArrayLayers */ 1,
            /* imageUsage */ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            /* imageSharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 1,
            /* pQueueFamilyIndices */ &graphQueueFamily,
            /* preTransform */ capabilities.currentTransform,
            /* compositeAlpha */ window->compositeAlpha,
            /* presentMode */ window->presentMode,
            /* clipped */ VK_TRUE,
            /* oldSwapchain */ oldSwapchain,
    };

    err = vkCreateSwapchainKHR(device, &swap_chain_create_info, VK_NULL_HANDLE, &window->swapchain);
    assert(!err);

    if (oldSwapchain)
        vkDestroySwapchainKHR(device, oldSwapchain, VK_NULL_HANDLE);

    /* initialize swap chain resources */
    window->swapchainResources = (SwapchainResource *) imalloc(sizeof(SwapchainResource) * window->imageBufferCount);

    std::vector<VkImage> swap_chain_images;
    swap_chain_images.resize(window->imageBufferCount);
    err = vkGetSwapchainImagesKHR(device, window->swapchain, &window->imageBufferCount, std::data(swap_chain_images));
    assert(!err);

    for (uint32_t i = 0; i < window->imageBufferCount; i++) {
        window->swapchainResources[i].image = swap_chain_images[i];

        VkCommandBufferAllocateInfo cmd_allocate_info = {
                /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                /* pNext */ VK_NULL_HANDLE,
                /* commandPool */ cmdPool,
                /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                /* commandBufferCount */ 1
        };

        rd->AllocateCommandBuffer(&(window->swapchainResources[i].cmdBuffer));

        VkImageViewCreateInfo image_view_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                /* pNext */ VK_NULL_HANDLE,
                /* flags */ VK_NONE_FLAGS,
                /* image */ window->swapchainResources[i].image,
                /* viewType */ VK_IMAGE_VIEW_TYPE_2D,
                /* format */ window->format,
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

        err = vkCreateImageView(device, &image_view_create_info, VK_NULL_HANDLE, &(window->swapchainResources[i].imageView));
        assert(!err);

        VkImageView framebuffer_attachments[] = { window->swapchainResources[i].imageView };

        VkFramebufferCreateInfo framebuffer_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                /* pNext */ VK_NULL_HANDLE,
                /* flags */ VK_NONE_FLAGS,
                /* renderPass */ window->renderPass,
                /* attachmentCount */ ARRAY_SIZE(framebuffer_attachments),
                /* pAttachments */ framebuffer_attachments,
                /* width */ window->width,
                /* height */ window->height,
                /* layers */ 1,
        };

        err = vkCreateFramebuffer(device, &framebuffer_create_info, VK_NULL_HANDLE, &(window->swapchainResources[i].framebuffer));
        assert(!err);
    }
}

void RenderScreen::_CleanUpSwapchain()
{
    for (uint32_t i = 0; i < window->imageBufferCount; i++) {
        vkFreeCommandBuffers(device, cmdPool, 1, &window->swapchainResources[i].cmdBuffer);
        vkDestroyFramebuffer(device, window->swapchainResources[i].framebuffer, VK_NULL_HANDLE);
        vkDestroyImageView(device, window->swapchainResources[i].imageView, VK_NULL_HANDLE);
    }

    free(window->swapchainResources);
}

void RenderScreen::_UpdateSwapchain()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, window->surface, &capabilities);
    VkExtent2D extent = capabilities.currentExtent;

    /* is update */
    if ((extent.width != window->width || extent.height != window->height) && (extent.width != 0 || extent.height != 0)) {
        vkDeviceWaitIdle(device);
        _CleanUpSwapchain();
        _CreateSwapchain();
    }
}
