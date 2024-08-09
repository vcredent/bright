/* ======================================================================== */
/* Main.cpp                                                                 */
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
#include "Win32/RenderDeviceContextWin32.h"
#include "Renderer/RenderDisplay.h"
#include <NavUI/NavUI.h>

int main()
{
    Window *window = new Window("BrightEngine", 1680, 1080);
    RenderDeviceContextWin32 *rdc = new RenderDeviceContextWin32(window);
    rdc->Initialize();
    RenderDevice *rd = rdc->CreateRenderDevice();

    RenderDisplay* display = memnew(RenderDisplay, rd);
    display->Initialize(window);

    NavUI::InitializeInfo initializeInfo = {};
    initializeInfo.window = (GLFWwindow *) window->GetNativeHandle();
    initializeInfo.Instance = rdc->GetInstance();
    initializeInfo.PhysicalDevice = rdc->GetPhysicalDevice();
    initializeInfo.Device = rdc->GetDevice();
    initializeInfo.QueueFamily = rdc->GetQueueFamily();
    initializeInfo.Queue = rdc->GetQueue();
    initializeInfo.DescriptorPool = rd->GetDescriptorPool();
    initializeInfo.RenderPass = display->GetRenderPass();
    initializeInfo.MinImageCount = display->GetImageBufferCount();
    initializeInfo.ImageCount = display->GetImageBufferCount();
    initializeInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    NavUI::Initialize(&initializeInfo);

    while (!window->IsClose()) 
    {
        window->PollEvents();

        VkCommandBuffer cmdBuffer;
        display->CmdBeginDisplayRender(&cmdBuffer);
        NavUI::BeginNewFrame(cmdBuffer);
        NavUI::EndNewFrame(cmdBuffer);
        display->CmdEndDisplayRender(cmdBuffer);
    }

    NavUI::Destroy();
    memdel(display);
    memdel(rd);
    memdel(rdc);
    memdel(window);

    return 0;
}