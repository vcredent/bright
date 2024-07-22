/* ======================================================================== */
/* render_device_context.cpp                                                */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           COPILOT ENGINE                                 *
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
#include "render_device_context.h"
#include <algorithm>

RenderDeviceContext::RenderDeviceContext()
{
    VkResult U_ASSERT_ONLY err;

    // ************************************************* //
    //                initialize instance                //
    // ************************************************* //
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

    // ******************************************************** //
    //                initialize physical device                //
    // ******************************************************** //
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

RenderDeviceContext::~RenderDeviceContext()
{
    vmaDestroyAllocator(allocator);
    vkDestroyCommandPool(device, command_pool, allocation_callbacks);
    vkDestroyDevice(device, allocation_callbacks);
    vkDestroyInstance(instance, allocation_callbacks);
}

Error RenderDeviceContext::initialize()
{
    _create_device();
    _create_command_pool();
    _create_vma_allocator();

    return OK;
}

void RenderDeviceContext::allocate_command_buffer(VkCommandBufferLevel level, VkCommandBuffer *p_command_buffer)
{
    VkResult U_ASSERT_ONLY err;

    VkCommandBufferAllocateInfo allocate_info = {
            /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            /* pNext */ nextptr,
            /* commandPool */ command_pool,
            /* level */ level,
            /* commandBufferCount */ 1
    };

    err = vkAllocateCommandBuffers(device, &allocate_info, p_command_buffer);
    assert(!err);
}

void RenderDeviceContext::free_command_buffer(VkCommandBuffer command_buffer)
{
    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void RenderDeviceContext::_initialize_window_arguments(VkSurfaceKHR surface)
{
    VkResult U_ASSERT_ONLY err;

    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);
    assert(!err);

    /* pick surface format */
    uint32_t foramt_count = 0;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &foramt_count, nullptr);
    assert(!err);

    VkSurfaceFormatKHR *surface_formats_khr = (VkSurfaceFormatKHR *) imalloc(sizeof(VkSurfaceFormatKHR) * foramt_count);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &foramt_count, surface_formats_khr);
    assert(!err);

    VkSurfaceFormatKHR surface_format = pick_surface_format(surface_formats_khr, foramt_count);
    format = surface_format.format;

    /* add queue create info */
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
    VkQueueFamilyProperties *queue_family_properties = (VkQueueFamilyProperties *) imalloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

    for (uint32_t i = 0; i < queue_family_count; i++) {
        VkQueueFamilyProperties properties = queue_family_properties[i];
        VkBool32 is_support_present = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &is_support_present);
        if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && is_support_present) {
            graph_queue_family = i;
            break;
        }
    }

    free(queue_family_properties);
}

void RenderDeviceContext::_create_device()
{
    VkResult U_ASSERT_ONLY err;

    float priorities = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* queueFamilyIndex */ graph_queue_family,
            /* queueCount */ 1,
            /* pQueuePriorities */ &priorities
    };

    /* create logic device */
    const char *extensions[] = {
            "VK_KHR_swapchain",
            "VK_KHR_synchronization2"
    };

    VkDeviceCreateInfo device_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* queueCreateInfoCount */ 1,
            /* pQueueCreateInfos */ &queue_create_info,
            /* enabledLayerCount */ 0,
            /* ppEnabledLayerNames */ nullptr,
            /* enabledExtensionCount */ ARRAY_SIZE(extensions),
            /* ppEnabledExtensionNames */ extensions,
            /* pEnabledFeatures */ nullptr,
    };

    err = vkCreateDevice(physical_device, &device_create_info, allocation_callbacks, &device);
    assert(!err);

    vkGetDeviceQueue(device, graph_queue_family, 0, &graph_queue);
}

void RenderDeviceContext::_create_command_pool()
{
    VkResult U_ASSERT_ONLY err;

    VkCommandPoolCreateInfo command_pool_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            /* queueFamilyIndex */ graph_queue_family
    };

    err = vkCreateCommandPool(device, &command_pool_create_info, allocation_callbacks, &command_pool);
    assert(!err);
}

void RenderDeviceContext::_create_vma_allocator()
{
    VkResult U_ASSERT_ONLY err;

    VmaAllocatorCreateInfo vma_allocator_create_info = {};
    vma_allocator_create_info.instance = instance;
    vma_allocator_create_info.physicalDevice = physical_device;
    vma_allocator_create_info.device = device;

    err = vmaCreateAllocator(&vma_allocator_create_info, &allocator);
    assert(!err);
}