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

VkAllocationCallbacks *allocation_callbacks = nullptr;

static VkSurfaceFormatKHR pick_surface_format(const VkSurfaceFormatKHR *surface_formats, uint32_t count)
{
    VkFormat format;
    for (uint32_t i = 0; i < count; i++) {
        format = surface_formats[i].format;
        if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM ||
                format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 || format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 ||
                format == VK_FORMAT_R16G16B16A16_SFLOAT)
            return surface_formats[i];
    }

    printf("Can't find our preferred formats... Falling back to first exposed format. Rendering may be incorrect.\n");

    assert(count >= 1);
    return surface_formats[0];
}

VulkanContext::VulkanContext()
{
    VkResult U_ASSERT_ONLY err;
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

    err = vkCreateInstance(&instance_create_info, allocation_callbacks, &inst);
    assert(!err);
}

VulkanContext::~VulkanContext()
{
    _clean_up_all();
}

void VulkanContext::_window_create(VkSurfaceKHR surface)
{
    _create_physical_device(surface);
    _create_device(&device);
    _create_command_pool(device);
    _initialize_window(gpu, surface);
    _create_swap_chain(device, nullptr, window);
    _allocate_command_buffers(device, command_pool, window);
}

void VulkanContext::_clean_up_all()
{
    // clean handle about display window.
    _clean_swap_chain(device, command_pool, window);
    vkDestroySurfaceKHR(inst, window->surface, allocation_callbacks);
    free(window);

    vkDestroyCommandPool(device, command_pool, allocation_callbacks);
    vkDestroyDevice(device, allocation_callbacks);
    vkDestroyInstance(inst, allocation_callbacks);
}

void VulkanContext::_create_physical_device(VkSurfaceKHR surface)
{
    VkResult U_ASSERT_ONLY err;

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

    _initialize_queues(gpu, surface);
}

void VulkanContext::_initialize_queues(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    VkResult U_ASSERT_ONLY err;
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
}

void VulkanContext::_create_device(VkDevice *p_device)
{
    void *nextptr = nullptr;
    float priorities = 1.0f;
    VkResult U_ASSERT_ONLY err;

    VkDeviceQueueCreateInfo queue_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ 0,
        /* queueFamilyIndex */ graph_queue_family,
        /* queueCount */ 1,
        /* pQueuePriorities */ &priorities
    };

    const char *extensions[] = {
        "VK_KHR_swapchain",
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
        /* enabledExtensionCount */ ARRAY_SIZE(extensions),
        /* ppEnabledExtensionNames */ extensions,
        /* pEnabledFeatures */ nullptr,
    };

    err = vkCreateDevice(gpu, &device_create_info, allocation_callbacks, p_device);
    assert(!err);
    vkGetDeviceQueue(device, graph_queue_family, 0, &graph_command_queue);
}

void VulkanContext::_create_command_pool(VkDevice device)
{
    void *nextptr = nullptr;
    VkResult U_ASSERT_ONLY err;

    VkCommandPoolCreateInfo command_pool_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ 0,
        /* queueFamilyIndex */ graph_queue_family
    };

    err = vkCreateCommandPool(device, &command_pool_create_info, allocation_callbacks, &command_pool);
    assert(!err);
}

void VulkanContext::_initialize_window(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    void *nextptr = nullptr;
    VkResult U_ASSERT_ONLY err;

    if (window)
        return;

    /* malloc display window struct and set surface */
    window = (Window *) malloc(sizeof(Window));
    window->surface = surface;

    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &window->capabilities);
    assert(!err);

    window->width = window->capabilities.currentExtent.width;
    window->height = window->capabilities.currentExtent.height;

    /* try to find surface format */
    uint32_t surface_format_count;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count, nullptr);
    assert(!err);

    if (surface_format_count <= 0)
        EXIT_FAIL("-engine error: vulkan surface image format error, no available surface format, format count: %d\n", surface_format_count);

    VkSurfaceFormatKHR *surface_image_formats = (VkSurfaceFormatKHR *) malloc(sizeof(VkSurfaceFormatKHR) * surface_format_count);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count, surface_image_formats);
    assert(!err);

    VkSurfaceFormatKHR final_surface_format = pick_surface_format(surface_image_formats, surface_format_count);
    window->format = final_surface_format.format;
    window->colorspace = final_surface_format.colorSpace;
    free(surface_image_formats);

    // try to set 3 buffer count of image
    uint32_t desired_buffer_count = 3;
    if (desired_buffer_count < window->capabilities.minImageCount)
        desired_buffer_count = window->capabilities.minImageCount;

    if ((window->capabilities.maxImageCount > 0) && (desired_buffer_count > window->capabilities.maxImageCount))
        desired_buffer_count = window->capabilities.maxImageCount;

    window->desired_buffer_count = desired_buffer_count;

    // surface pre transform
    window->transform = (window->capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ?
            VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : window->capabilities.currentTransform;

    // composite alpha
    window->composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // present mode
    window->present_mode = VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanContext::_create_swap_chain(VkDevice device, VkSwapchainKHR old_swap_chain, Window *window)
{
    void *nextptr = nullptr;
    VkResult U_ASSERT_ONLY err;

    VkSwapchainCreateInfoKHR swapchain_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            /* pNext */ nextptr,
            /* flags */ 0,
            /* surface */ window->surface,
            /* minImageCount */ window->desired_buffer_count,
            /* imageFormat */ window->format,
            /* imageColorSpace */ window->colorspace,
            /* imageExtent */
                {
                    .width = window->width,
                    .height = window->height,
                },
            /* imageArrayLayers */ 1,
            /* imageUsage */ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            /* imageSharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 1,
            /* pQueueFamilyIndices */ &graph_queue_family,
            /* preTransform */ window->transform,
            /* compositeAlpha */ window->composite_alpha,
            /* presentMode */ window->present_mode,
            /* clipped */ VK_TRUE,
            /* oldSwapchain */ old_swap_chain,
    };

    err = vkCreateSwapchainKHR(device, &swapchain_create_info, allocation_callbacks, &window->swapchain);
    assert(!err);
}

void VulkanContext::_clean_swap_chain(VkDevice device, VkCommandPool command_pool, Window *window)
{
    vkDestroySwapchainKHR(device, window->swapchain, allocation_callbacks);
    window->swapchain = nullptr;
    vkFreeCommandBuffers(device, command_pool, window->desired_buffer_count, window->command_buffers);
    window->command_buffers = nullptr;
}

void VulkanContext::_allocate_command_buffers(VkDevice device, VkCommandPool command_pool, Window *window)
{
    void *nextptr = nullptr;
    VkResult U_ASSERT_ONLY err;

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /* pNext */ nextptr,
        /* commandPool */ command_pool,
        /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /* commandBufferCount */ window->desired_buffer_count,
    };

    window->command_buffers = (VkCommandBuffer *) malloc(sizeof(VkCommandBuffer) * window->desired_buffer_count);
    err = vkAllocateCommandBuffers(device, &command_buffer_allocate_info, window->command_buffers);
    assert(!err);
}