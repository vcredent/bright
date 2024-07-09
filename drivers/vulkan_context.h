/* ======================================================================== */
/* vulkan_context.h                                                         */
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
#include <vulkan/vulkan.h>
#include "hardware.h"

class VulkanContext : public HardwareDevice {
public:
    explicit VulkanContext();
    ~VulkanContext() override;

public:
    VkInstance get_instance() { return inst; }

protected:
    void _window_create(VkSurfaceKHR surface); /* initialize */

private:
    void _create_physical_device(VkSurfaceKHR surface);
    void _create_device(VkDevice *p_device);

private:
    VkInstance inst;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;
    VkDevice device;
    uint32_t graph_queue_family;
    VkQueue graph_command_queue;
};
