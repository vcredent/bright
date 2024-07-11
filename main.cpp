/* ======================================================================== */
/* main.cpp                                                                 */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           PORTABLE ENGINE                                */
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
#include <iostream>
#include "window/vulkan_context_win32.h"
#include "drivers/rendering_device_driver_vulkan.h"

int main(int argc, char **argv)
{
    VkContextWin32 *vkctx_win32 = new VkContextWin32();
    vkctx_win32->window_create(800, 600, "portable");
    RenderingDeviceDriverVulkan *driver = new RenderingDeviceDriverVulkan(vkctx_win32);

    while (!vkctx_win32->window_should_close()) {
        vkctx_win32->poll_events();
    }

    delete vkctx_win32;
    delete driver;

    return 0;
}