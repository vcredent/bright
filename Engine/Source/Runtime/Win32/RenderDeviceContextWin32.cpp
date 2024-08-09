/* ======================================================================== */
/* render_device_context_win32.cpp                                          */
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
#include "RenderDeviceContextWin32.h"

RenderDeviceContextWin32::RenderDeviceContextWin32(Window *window)
{
    VkSurfaceKHR surface;
    window->CreateWindowSurfaceKHR(GetInstance(), VK_NULL_HANDLE, &surface);
    _InitializeWindowArguments(surface);
    vkDestroySurfaceKHR(GetInstance(), surface, VK_NULL_HANDLE);
}

RenderDeviceContextWin32::~RenderDeviceContextWin32()
{
    /* do nothing in here... */
}

RenderDevice *RenderDeviceContextWin32::CreateRenderDevice()
{
    return memnew(RenderDevice, this);
}

void RenderDeviceContextWin32::DestroyRenderDevice(RenderDevice * pRenderDevice)
{
    memdel(pRenderDevice);
}