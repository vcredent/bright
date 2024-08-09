/* ======================================================================== */
/* RenderDisplay.h                                                          */
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
#ifndef _RENDERING_SCREEN_H_
#define _RENDERING_SCREEN_H_

#include "Runtime/Drivers/RenderDevice.h"
#include "Runtime/Window/Window.h"

class RenderDisplay {
public:
    RenderDisplay(RenderDevice *vRD, Window *vWindow);
   ~RenderDisplay();

    VkRenderPass GetRenderPass() { return display->renderPass; }
    uint32_t GetImageBufferCount() { return display->imageBufferCount; }
    Window *GetNativeWindow() { return currentNativeWindow; }

    void CmdBeginDisplayRender(VkCommandBuffer *pCmdBuffer);
    void CmdEndDisplayRender(VkCommandBuffer cmdBuffer);

private:
    struct SwapchainResource {
        VkCommandBuffer cmdBuffer;
        VkImage image;
        VkImageView imageView;
        VkFramebuffer framebuffer;
    };

    struct Display {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkFormat format;
        VkColorSpaceKHR colorSpace;
        uint32_t imageBufferCount;
        VkCompositeAlphaFlagBitsKHR compositeAlpha;
        VkPresentModeKHR presentMode;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        SwapchainResource *swapchainResources;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        uint32_t width;
        uint32_t height;
    };

    void _Initialize();
    void _CreateSwapchain();
    void _CleanUpSwapchain();
    void _CheckUpdateSwapchain();

    RenderDevice *rd = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t graphQueueFamily = 0;
    VkCommandPool cmdPool = VK_NULL_HANDLE;
    Display *display = VK_NULL_HANDLE;
    VkQueue graphQueue = VK_NULL_HANDLE;
    Window *currentNativeWindow= VK_NULL_HANDLE;

    uint32_t acquireNextIndex;
};

#endif /* _RENDERING_SCREEN_H_ */
