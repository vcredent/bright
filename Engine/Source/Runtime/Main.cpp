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
#include "Rendering/RenderingScreen.h"
#include <NavUI/NavUI.h>

int main()
{
    Window *window = new Window("Bright", 1680, 1080);
    RenderDeviceContextWin32 *rdc = new RenderDeviceContextWin32(window);
    rdc->Initialize();
    RenderDevice *rd = rdc->CreateRenderDevice();

    RenderingScreen *screen = new RenderingScreen(rd);
    screen->Initialize(window);

    NavUI::InitializeInfo initialize_info = {};
    initialize_info.window = (GLFWwindow *) window->GetNativeHandle();
    initialize_info.Instance = rdc->GetInstance();
    initialize_info.PhysicalDevice = rdc->GetPhysicalDevice();
    initialize_info.Device = rdc->GetDevice();
    initialize_info.QueueFamily = rdc->GetQueueFamily();
    initialize_info.Queue = rdc->GetQueue();
    initialize_info.DescriptorPool = rd->GetDescriptorPool();
    initialize_info.RenderPass = screen->GetRenderPass();
    initialize_info.MinImageCount = screen->GetImageBufferCount();
    initialize_info.ImageCount = screen->GetImageBufferCount();
    initialize_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    NavUI::Initialize(&initialize_info);

    while (!window->IsClose()) {
        window->PollEvents();

        VkCommandBuffer cmdBuffer;
        screen->CmdBeginScreenRender(&cmdBuffer);
        {
            NavUI::BeginNewFrame(cmdBuffer);
            NavUI::Begin("Hello");
            NavUI::End();
            NavUI::EndNewFrame(cmdBuffer);
        }
        screen->CmdEndScreenRender(cmdBuffer);
    }

    NavUI::Destroy();
    memdel(screen);
    memdel(rd);
    memdel(rdc);
    memdel(window);

    return 0;
}