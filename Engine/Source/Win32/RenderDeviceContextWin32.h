/* ======================================================================== */
/* render_device_context_win32.h                                            */
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
#ifndef _RENDERING_CONTEXT_DRIVER_VULKAN_WIN32_H
#define _RENDERING_CONTEXT_DRIVER_VULKAN_WIN32_H

#include "Drivers/RenderDevice.h"
#include "Window/window.h"

// Render driver context for vulkan
class RenderDeviceContextWin32 : public RenderDeviceContext {
public:
    RenderDeviceContextWin32(Window *window);
    ~RenderDeviceContextWin32();

    RenderDevice *CreateRenderDevice();
    void DestroyRenderDevice(RenderDevice *p_render_device);
};

#endif /* _RENDERING_CONTEXT_DRIVER_VULKAN_WIN32_H */
