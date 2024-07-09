/* ======================================================================== */
/* vulkan_context.cpp                                                       */
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
#include "vulkan_context.h"
#include <vector>

VulkanContext::VulkanContext()
{
    VkResult ASSERT_ONLY err;
    void *nextptr = nullptr;

    VkApplicationInfo application_info = {
        /* sType */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
        /* pNext */ nextptr,
        /* pApplicationName */ "PortableX",
        /* applicationVersion */ VK_MAKE_VERSION(1, 0, 0),
        /* pEngineName */ "PortableEngine",
        /* engineVersion */ VK_MAKE_VERSION(1, 0, 0),
        /* apiVersion */ VK_API_VERSION_1_3
    };

    std::vector<const char *> inst_extensions = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface"
    };

    std::vector<const char *> inst_layers = {
            "VK_LAYER_KHRONOS_validation" /* must install Vulkan SDK. */
    };

    VkInstanceCreateInfo instance_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ 0,
        /* pApplicationInfo */ &application_info,
        /* enabledLayerCount */ (uint32_t) std::size(inst_layers),
        /* ppEnabledLayerNames */ std::data(inst_layers),
        /* enabledExtensionCount */ (uint32_t) std::size(inst_extensions),
        /* ppEnabledExtensionNames */ std::data(inst_extensions)
    };

    err = vkCreateInstance(&instance_create_info, nullptr, &inst);
    assert(!err);
}

VulkanContext::~VulkanContext()
{
    VkAllocationCallbacks *allocation_callbacks = nullptr;
    vkDestroyDevice(device, allocation_callbacks);
    vkDestroySurfaceKHR(inst, surface, allocation_callbacks);
    vkDestroyInstance(inst, allocation_callbacks);
}

void VulkanContext::_window_create(VkSurfaceKHR surface)
{
    _create_physical_device(surface);
    _create_device(&device);
}

void VulkanContext::_create_physical_device(VkSurfaceKHR surface)
{
    VkResult ASSERT_ONLY err;

    /* select high performance GPU in this machine. */
    uint32_t gpu_count;
    uint32_t gpu_number = 0;
    err = vkEnumeratePhysicalDevices(inst, &gpu_count, nullptr);
    assert(!err);

    if (gpu_count <= 0)
        EXIT_FAIL("-engine error: no available device (GPU), gpu count is: %d\n", gpu_count);

    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *) malloc(sizeof(VkPhysicalDevice) * gpu_count);
    err = vkEnumeratePhysicalDevices(inst, &gpu_count, physical_devices);
    assert(!err);

    VkPhysicalDeviceProperties physical_device_properties;
    for (int i = 0; i < gpu_count; i++) {
        vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
        if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            gpu_number = i;
            break;
        }
    }

    gpu = physical_devices[gpu_number];
    free(physical_devices);

    /* find graphics and present queue family. */
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);
    VkQueueFamilyProperties *queue_family_properties = (VkQueueFamilyProperties *) malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_family_properties);

    // search for a graphics and present queue in the array of queue
    // families, try to find one that supports both.
    for (uint32_t i = 0; i < queue_family_count; i++) {
        VkBool32 is_present;
        err = vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &is_present);
        assert(!err);
        if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (is_present == VK_TRUE)) {
            graph_queue_family = i;
            break;
        }
    }

    free(queue_family_properties);

    this->surface = surface;
}

void VulkanContext::_create_device(VkDevice *p_device)
{
    void *nextptr = nullptr;
    float priorities = 1.0f;
    VkResult ASSERT_ONLY err;

    VkDeviceQueueCreateInfo queue_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ 0,
        /* queueFamilyIndex */ graph_queue_family,
        /* queueCount */ 1,
        /* pQueuePriorities */ &priorities
    };

    /* create device. */
    VkDeviceCreateInfo device_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ 0,
        /* queueCreateInfoCount */ 1,
        /* pQueueCreateInfos */ &queue_create_info,
        /* enabledLayerCount */ 0,
        /* ppEnabledLayerNames */ nullptr,
        /* enabledExtensionCount */ 0,
        /* ppEnabledExtensionNames */ nullptr,
        /* pEnabledFeatures */ nullptr,
    };

    err = vkCreateDevice(gpu, &device_create_info, nullptr, p_device);
    assert(!err);
    vkGetDeviceQueue(device, graph_queue_family, 0, &graph_command_queue);
}