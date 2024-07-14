/* ======================================================================== */
/* rendering_context_driver_vulkan.cpp                                      */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           PORTABLE ENGINE                                *
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
#include "rendering_context_driver_vulkan.h"
#include <algorithm>

RenderingContextDriverVulkan::RenderingContextDriverVulkan()
{
    // nothing in here...
}

RenderingContextDriverVulkan::~RenderingContextDriverVulkan()
{
    vkDestroyInstance(instance, allocation_callbacks);
    vkDestroyDevice(device, allocation_callbacks);
}

Error RenderingContextDriverVulkan::initialize()
{
    _create_instance();
    _initialize_physical_device();

    return OK;
}

void RenderingContextDriverVulkan::_create_instance()
{
    VkResult U_ASSERT_ONLY err;

    VkApplicationInfo application_info = {
            /* sType */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
            /* pNext */ nextptr,
            /* pApplicationName */ "PortableX",
            /* applicationVersion */ VK_MAKE_VERSION(1, 0, 0),
            /* pEngineName */ "PortableEngine",
            /* engineVersion */ VK_MAKE_VERSION(1, 0, 0),
            /* apiVersion */ VK_API_VERSION_1_3
    };

    const char * extensions[] = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface"
    };

    const char * layers[] = {
            "VK_LAYER_KHRONOS_validation" /* required install Vulkan SDK */
    };

    VkInstanceCreateInfo instance_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* pApplicationInfo */ &application_info,
            /* enabledLayerCount */ ARRAY_SIZE(layers),
            /* ppEnabledLayerNames */ layers,
            /* enabledExtensionCount */ ARRAY_SIZE(extensions),
            /* ppEnabledExtensionNames */ extensions
    };

    err = vkCreateInstance(&instance_create_info, allocation_callbacks, &instance);
    assert(!err);
}

void RenderingContextDriverVulkan::_initialize_physical_device()
{
    VkResult U_ASSERT_ONLY err;

    /* select high performance GPU in this machine. */
    uint32_t gpu_count;
    uint32_t gpu_number = 0;
    err = vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);
    assert(!err);

    if (gpu_count <= 0)
        EXIT_FAIL("-engine error: no available device (GPU), gpu count is: %d\n", gpu_count);

    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *) imalloc(sizeof(VkPhysicalDevice) * gpu_count);
    err = vkEnumeratePhysicalDevices(instance, &gpu_count, physical_devices);
    assert(!err);

    VkPhysicalDeviceProperties physical_device_properties;
    for (int i = 0; i < gpu_count; i++) {
        vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
        if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            gpu_number = i;
            break;
        }
    }

    physical_device = physical_devices[gpu_number];
    free(physical_devices);

    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);
}