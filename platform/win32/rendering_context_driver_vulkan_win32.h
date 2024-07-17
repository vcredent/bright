/* ======================================================================== */
/* rendering_context_driver_vulkan_win32.h                                  */
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
#ifndef _RENDERING_CONTEXT_DRIVER_VULKAN_WIN32_H
#define _RENDERING_CONTEXT_DRIVER_VULKAN_WIN32_H

#include "drivers/rendering_device_driver_vulkan.h"
#include <GLFW/glfw3.h>

// Render driver context for vulkan
class RenderingContextDriverVulkanWin32 : public RenderingContextDriverVulkan {
public:
    RenderingContextDriverVulkanWin32(GLFWwindow *window);
    ~RenderingContextDriverVulkanWin32();

    RenderingDeviceDriverVulkan *load_render_device();
    void destroy_render_device(RenderingDeviceDriverVulkan *p_render_device);
};

#endif /* _RENDERING_CONTEXT_DRIVER_VULKAN_WIN32_H */
