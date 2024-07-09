/* ======================================================================== */
/* vulkan_context_win32.cpp                                                 */
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
#include "vulkan_context_win32.h"

VulkanContextWin32::VulkanContextWin32()
{
    if (!glfwInit())
        EXIT_FAIL("-engine error: glfw init error!");
}

VulkanContextWin32::~VulkanContextWin32()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void VulkanContextWin32::window_create(int width, int height, const char *title, struct hardware_device_hint *p_hint)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, p_hint->window_visible);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    EXIT_FAIL_V(window != NULL, "-engine error: create glfw window failed! cause: window is null.");

    /* create surface. */
    VkSurfaceKHR surface;
    VkResult ASSERT_ONLY err;
    err = glfwCreateWindowSurface(get_instance(), window, NULL, &surface);
    assert(!err);

    _window_create(surface);
}

bool VulkanContextWin32::window_should_close()
{
    return glfwWindowShouldClose(window);
}

void VulkanContextWin32::poll_events()
{
    glfwPollEvents();
}