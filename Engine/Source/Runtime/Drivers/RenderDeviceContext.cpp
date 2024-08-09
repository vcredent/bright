/* ======================================================================== */
/* RenderDeviceContext.cpp                                                  */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                            BRIGHT ENGINE                                 *
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
#include "RenderDeviceContext.h"
#include <algorithm>

const char *ignoreValidationError[] = {
        "NONE",
        /* "[ VUID-RuntimeSpirv-samples-08725 ]", */
        /* "[ VUID-VkWriteDescriptorSet-descriptorType-04150 ]", */
};

#ifdef ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    const char *message = pCallbackData->pMessage;

    // check ignore error.
    for (uint32_t i = 0; i < ARRAY_SIZE(ignoreValidationError); i++) {
        if (strstr(message, ignoreValidationError[i]) != NULL) {
            goto VALIDATION_DEBUG_CALLBACK_END;
        }
    }

    fprintf(stderr, "%s\n", message);
VALIDATION_DEBUG_CALLBACK_END:
    return VK_FALSE;
}
#endif

RenderDeviceContext::RenderDeviceContext()
{
    VkResult U_ASSERT_ONLY err;

    // ************************************************* //
    //                initialize instance                //
    // ************************************************* //
    VkApplicationInfo application_info = {
            /* sType */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* pApplicationName */ "PortableX",
            /* applicationVersion */ VK_MAKE_VERSION(1, 0, 0),
            /* pEngineName */ "PortableEngine",
            /* engineVersion */ VK_MAKE_VERSION(1, 0, 0),
            /* apiVersion */ VK_API_VERSION_1_3
    };

    const char * extensions[] = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface",
#if defined(ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT)
            "VK_EXT_debug_utils",
#endif
    };

    const char * layers[] = {
#if defined(ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT)
            "VK_LAYER_KHRONOS_validation",
#endif
    };

    VkInstanceCreateInfo instance_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* pApplicationInfo */ &application_info,
            /* enabledLayerCount */ ARRAY_SIZE(layers),
            /* ppEnabledLayerNames */ layers,
            /* enabledExtensionCount */ ARRAY_SIZE(extensions),
            /* ppEnabledExtensionNames */ extensions
    };

    err = vkCreateInstance(&instance_create_info, VK_NULL_HANDLE, &instance);
    assert(!err);

    _LoadVulkanFunctionProcAddr();

    // ******************************************************** //
    //                initialize debug messenger                //
    // ******************************************************** //
#ifdef ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT
    VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = {};
    messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messenger_create_info.pfnUserCallback = debugCallback;

    err = fnCreateDebugUtilsMessengerEXT(instance, &messenger_create_info, VK_NULL_HANDLE, &messenger);
    assert(!err);
#endif

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

    for (int i = 0; i < gpu_count; i++) {
        vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
        if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            gpu_number = i;
            break;
        }
    }

    physicalDevice = physical_devices[gpu_number];
    free(physical_devices);

    vkGetPhysicalDeviceProperties(physicalDevice, &physical_device_properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &physical_device_features);

    max_msaa_sample_counts = find_max_msaa_sample_counts(physical_device_properties);
}

RenderDeviceContext::~RenderDeviceContext()
{
    vmaDestroyAllocator(allocator);
    vkDestroyCommandPool(device, cmd_pool, VK_NULL_HANDLE);
    vkDestroyDevice(device, VK_NULL_HANDLE);
#ifdef ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT
    fnDestroyDebugUtilsMessengerExt(instance, messenger, VK_NULL_HANDLE);
#endif
    vkDestroyInstance(instance, VK_NULL_HANDLE);
}

VkFormat RenderDeviceContext::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    throw std::runtime_error("failed to find supported format!");
}


void RenderDeviceContext::AllocateCommandBuffer(VkCommandBufferLevel level, VkCommandBuffer *pCmdBuffer)
{
    VkResult U_ASSERT_ONLY err;

    VkCommandBufferAllocateInfo allocate_info = {
            /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* commandPool */ cmd_pool,
            /* level */ level,
            /* commandBufferCount */ 1
    };

    err = vkAllocateCommandBuffers(device, &allocate_info, pCmdBuffer);
    assert(!err);
}

void RenderDeviceContext::FreeCommandBuffer(VkCommandBuffer cmdBuffer)
{
    vkFreeCommandBuffers(device, cmd_pool, 1, &cmdBuffer);
}

void RenderDeviceContext::_Initialize(VkSurfaceKHR surface)
{
    VkResult U_ASSERT_ONLY err;

    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    assert(!err);

    /* pick surface format */
    uint32_t foramt_count = 0;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &foramt_count, nullptr);
    assert(!err);

    VkSurfaceFormatKHR *surface_formats_khr = (VkSurfaceFormatKHR *) imalloc(sizeof(VkSurfaceFormatKHR) * foramt_count);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &foramt_count, surface_formats_khr);
    assert(!err);

    VkSurfaceFormatKHR surface_format = pick_surface_format(surface_formats_khr, foramt_count);
    format = surface_format.format;

    /* add queue create info */
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, nullptr);
    VkQueueFamilyProperties *queue_family_properties = (VkQueueFamilyProperties *) imalloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, queue_family_properties);

    for (uint32_t i = 0; i < queue_family_count; i++) {
        VkQueueFamilyProperties properties = queue_family_properties[i];
        VkBool32 is_support_present = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &is_support_present);
        if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && is_support_present) {
            graph_queue_family = i;
            break;
        }
    }

    free(queue_family_properties);

    // create device...
    _CreateDevice();
    _CreateCommandPool();
    _CreateVmaAllocator();
}

void RenderDeviceContext::_LoadVulkanFunctionProcAddr()
{
#if defined(ENGINE_ENABLE_VULKAN_DEBUG_UTILS_EXT)
    fnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    fnDestroyDebugUtilsMessengerExt = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
#endif
}

void RenderDeviceContext::_CreateDevice()
{
    VkResult U_ASSERT_ONLY err;

    float priorities = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* queueFamilyIndex */ graph_queue_family,
            /* queueCount */ 1,
            /* pQueuePriorities */ &priorities
    };

    /* create logic device */
    const char *extensions[] = {
            "VK_KHR_swapchain",
            "VK_KHR_synchronization2"
    };

    VkPhysicalDeviceFeatures features = {};
    features.wideLines = VK_TRUE;

    VkDeviceCreateInfo device_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_NONE_FLAGS,
            /* queueCreateInfoCount */ 1,
            /* pQueueCreateInfos */ &queue_create_info,
            /* enabledLayerCount */ 0,
            /* ppEnabledLayerNames */ nullptr,
            /* enabledExtensionCount */ ARRAY_SIZE(extensions),
            /* ppEnabledExtensionNames */ extensions,
            /* pEnabledFeatures */ &features,
    };

    err = vkCreateDevice(physicalDevice, &device_create_info, VK_NULL_HANDLE, &device);
    assert(!err);

    vkGetDeviceQueue(device, graph_queue_family, 0, &graph_queue);
}

void RenderDeviceContext::_CreateCommandPool()
{
    VkResult U_ASSERT_ONLY err;

    VkCommandPoolCreateInfo cmd_pool_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            /* pNext */ VK_NULL_HANDLE,
            /* flags */ VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            /* queueFamilyIndex */ graph_queue_family
    };

    err = vkCreateCommandPool(device, &cmd_pool_create_info, VK_NULL_HANDLE, &cmd_pool);
    assert(!err);
}

void RenderDeviceContext::_CreateVmaAllocator()
{
    VkResult U_ASSERT_ONLY err;

    VmaAllocatorCreateInfo vma_allocator_create_info = {};
    vma_allocator_create_info.instance = instance;
    vma_allocator_create_info.physicalDevice = physicalDevice;
    vma_allocator_create_info.device = device;

    err = vmaCreateAllocator(&vma_allocator_create_info, &allocator);
    assert(!err);
}