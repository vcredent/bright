/* ======================================================================== */
/* render_device_context_win32.cpp                                          */
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
#include "render_device_context_win32.h"

RenderDeviceContextWin32::RenderDeviceContextWin32(GLFWwindow *window)
{
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(get_instance(), window, allocation_callbacks, &surface);
    _initialize_window_arguments(surface);
    vkDestroySurfaceKHR(get_instance(), surface, allocation_callbacks);
}

RenderDeviceContextWin32::~RenderDeviceContextWin32()
{
    /* do nothing in here... */
}

RenderDevice *RenderDeviceContextWin32::load_render_device()
{
    return memnew(RenderDevice, this);
}

void RenderDeviceContextWin32::destroy_render_device(RenderDevice *p_render_device)
{
    memdel(p_render_device);
}