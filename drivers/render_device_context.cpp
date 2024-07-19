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
    _clean_up_window();
    vmaDestroyAllocator(allocator);
    vkDestroyCommandPool(device, command_pool, allocation_callbacks);
    vkDestroyDevice(device, allocation_callbacks);
    vkDestroyInstance(instance, allocation_callbacks);
}

Error RenderDeviceContext::initialize()
{
    _create_device();
    _initialize_window_semaphore();
    _create_command_pool();
    _create_vma_allocator();
    _create_swap_chain();

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

void
RenderDeviceContext::get_window_semaphore(VkSemaphore *p_available_semaphore, VkSemaphore *p_finished_semaphore)
{
    *p_available_semaphore = window->image_available_semaphore;
    *p_finished_semaphore = window->render_finished_semaphore;
}

void RenderDeviceContext::acquire_next_image(RenderDeviceContext::AcquiredNext *p_acquired_next)
{
    p_acquired_next->swap_chain = window->swap_chain;
    p_acquired_next->wait_semaphore = window->image_available_semaphore;
    p_acquired_next->render_pass = window->render_pass;

    VkResult U_ASSERT_ONLY err;
    err = vkAcquireNextImageKHR(device, window->swap_chain, UINT32_MAX, p_acquired_next->wait_semaphore, VK_NULL_HANDLE, &p_acquired_next->index);
    assert(!err);

    p_acquired_next->command_buffer = window->swap_chain_resources[p_acquired_next->index].command_buffer;
    p_acquired_next->framebuffer = window->swap_chain_resources[p_acquired_next->index].framebuffer;
    p_acquired_next->width = window->width;
    p_acquired_next->height = window->height;
}

void RenderDeviceContext::free_command_buffer(VkCommandBuffer command_buffer)
{
    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void RenderDeviceContext::update_window()
{
    _update_swap_chain();
}

void RenderDeviceContext::_initialize_window(VkSurfaceKHR surface)
{
    VkResult U_ASSERT_ONLY err;

    /* imalloc display window struct and set surface */
    window = (Window *) imalloc(sizeof(Window));
    window->surface = surface;

    VkSurfaceCapabilitiesKHR capabilities;
    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, window->surface, &capabilities);
    assert(!err);

    /* pick surface format */
    uint32_t foramt_count = 0;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, window->surface, &foramt_count, nullptr);
    assert(!err);

    VkSurfaceFormatKHR *surface_formats_khr = (VkSurfaceFormatKHR *) imalloc(sizeof(VkSurfaceFormatKHR) * foramt_count);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, window->surface, &foramt_count, surface_formats_khr);
    assert(!err);

    VkSurfaceFormatKHR surface_format = pick_surface_format(surface_formats_khr, foramt_count);
    window->format = surface_format.format;
    window->color_space = surface_format.colorSpace;

    free(surface_formats_khr);

    /* image buffer count */
    uint32_t desired_buffer_count = 3;
    desired_buffer_count = std::clamp(desired_buffer_count, capabilities.minImageCount, capabilities.maxImageCount);
    window->image_buffer_count = desired_buffer_count;

    window->composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    window->present_mode = VK_PRESENT_MODE_FIFO_KHR;
}

void RenderDeviceContext::_clean_up_window()
{
    vkDestroySemaphore(device, window->image_available_semaphore, allocation_callbacks);
    vkDestroySemaphore(device, window->render_finished_semaphore, allocation_callbacks);
    vkDestroySwapchainKHR(device, window->swap_chain, allocation_callbacks);
    vkDestroyRenderPass(device, window->render_pass, allocation_callbacks);
    _clean_up_swap_chain();
    vkDestroySurfaceKHR(instance, window->surface, allocation_callbacks);
    free(window);
}

void RenderDeviceContext::_create_device()
{
    VkResult U_ASSERT_ONLY err;

    /* add queue create info */
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
    VkQueueFamilyProperties *queue_family_properties = (VkQueueFamilyProperties *) imalloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

    for (uint32_t i = 0; i < queue_family_count; i++) {
        VkQueueFamilyProperties properties = queue_family_properties[i];

        VkBool32 is_support_present = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, window->surface, &is_support_present);
        if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && is_support_present) {
            graph_queue_family = i;
            break;
        }
    }

    free(queue_family_properties);

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
            "VK_KHR_swapchain"
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

void RenderDeviceContext::_initialize_window_semaphore()
{
    VkResult U_ASSERT_ONLY err;

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    err = vkCreateSemaphore(device, &semaphore_create_info, allocation_callbacks, &window->image_available_semaphore);
    assert(!err);

    err = vkCreateSemaphore(device, &semaphore_create_info, allocation_callbacks, &window->render_finished_semaphore);
    assert(!err);
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

void RenderDeviceContext::_create_swap_chain()
{
    VkResult U_ASSERT_ONLY err;
    VkSwapchainKHR old_swap_chain;

    old_swap_chain = window->swap_chain;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, window->surface, &capabilities);
    window->width = capabilities.currentExtent.width;
    window->height = capabilities.currentExtent.height;
    window->aspect_ratio = window->width / window->height;

    if (!old_swap_chain) {
        // attachment
        VkAttachmentDescription attachment_description = {};
        attachment_description.format = window->format;
        attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // subpass
        VkAttachmentReference color_attachment = {};
        color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass_description = {};
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_attachment;

        // create render pass
        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &attachment_description;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass_description;

        err = vkCreateRenderPass(device, &render_pass_create_info, allocation_callbacks, &window->render_pass);
        assert(!err);
    }

    /* create swap chain */
    VkSwapchainCreateInfoKHR swap_chain_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* surface */ window->surface,
            /* minImageCount */ window->image_buffer_count,
            /* imageFormat */ window->format,
            /* imageColorSpace */ window->color_space,
            /* imageExtent */ capabilities.currentExtent,
            /* imageArrayLayers */ 1,
            /* imageUsage */ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            /* imageSharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 1,
            /* pQueueFamilyIndices */ &graph_queue_family,
            /* preTransform */ capabilities.currentTransform,
            /* compositeAlpha */ window->composite_alpha,
            /* presentMode */ window->present_mode,
            /* clipped */ VK_TRUE,
            /* oldSwapchain */ old_swap_chain,
    };

    err = vkCreateSwapchainKHR(device, &swap_chain_create_info, allocation_callbacks, &window->swap_chain);
    assert(!err);

    if (old_swap_chain)
        vkDestroySwapchainKHR(device, old_swap_chain, allocation_callbacks);

    /* initialize swap chain resources */
    window->swap_chain_resources = (SwapchainResource *) imalloc(sizeof(SwapchainResource) * window->image_buffer_count);

    VkImage swap_chain_images[window->image_buffer_count];
    err = vkGetSwapchainImagesKHR(device, window->swap_chain, &window->image_buffer_count, swap_chain_images);
    assert(!err);

    for (uint32_t i = 0; i < window->image_buffer_count; i++) {
        window->swap_chain_resources[i].image = swap_chain_images[i];

        VkCommandBufferAllocateInfo command_allocate_info = {
                /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                /* pNext */ nextptr,
                /* commandPool */ command_pool,
                /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                /* commandBufferCount */ 1
        };

        vkAllocateCommandBuffers(device, &command_allocate_info, &(window->swap_chain_resources[i].command_buffer));

        VkImageViewCreateInfo image_view_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                /* pNext */ nextptr,
                /* flags */ no_flag_bits,
                /* image */ window->swap_chain_resources[i].image,
                /* viewType */ VK_IMAGE_VIEW_TYPE_2D,
                /* format */ window->format,
                /* components */
                    {
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                    },
                /* subresourceRange */
                    {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                    },
        };

        err = vkCreateImageView(device, &image_view_create_info, allocation_callbacks, &(window->swap_chain_resources[i].image_view));
        assert(!err);

        VkImageView framebuffer_attachments[] = { window->swap_chain_resources[i].image_view };

        VkFramebufferCreateInfo framebuffer_create_info = {
                /* sType */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                /* pNext */ nextptr,
                /* flags */ no_flag_bits,
                /* renderPass */ window->render_pass,
                /* attachmentCount */ ARRAY_SIZE(framebuffer_attachments),
                /* pAttachments */ framebuffer_attachments,
                /* width */ window->width,
                /* height */ window->height,
                /* layers */ 1,
        };

        err = vkCreateFramebuffer(device, &framebuffer_create_info, allocation_callbacks, &(window->swap_chain_resources[i].framebuffer));
        assert(!err);
    }
}

void RenderDeviceContext::_clean_up_swap_chain()
{
    for (uint32_t i = 0; i < window->image_buffer_count; i++) {
        vkFreeCommandBuffers(device, command_pool, 1, &window->swap_chain_resources[i].command_buffer);
        vkDestroyFramebuffer(device, window->swap_chain_resources[i].framebuffer, allocation_callbacks);
        vkDestroyImageView(device, window->swap_chain_resources[i].image_view, allocation_callbacks);
    }

    free(window->swap_chain_resources);
}

void RenderDeviceContext::_update_swap_chain()
{
    vkDeviceWaitIdle(device);
    _clean_up_swap_chain();
    _create_swap_chain();
}