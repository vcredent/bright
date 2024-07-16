/* ======================================================================== */
/* rendering_device_driver_vulkan.h                                         */
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
#ifndef _RENDERING_DEVICE_DRIVER_VULKAN_H_
#define _RENDERING_DEVICE_DRIVER_VULKAN_H_

#include "vulkan_context.h"

class RenderingDeviceDriverVulkan {
public:
    RenderingDeviceDriverVulkan(VkContext *context);
    ~RenderingDeviceDriverVulkan();

private:
    VkContext *vk_context = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VmaAllocator allocator = VK_NULL_HANDLE;
};

#endif /* _RENDERING_DEVICE_DRIVER_VULKAN_H_ */