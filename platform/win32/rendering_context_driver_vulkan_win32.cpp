/* ======================================================================== */
/* rendering_context_driver_vulkan_win32.cpp                                */
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
#include "rendering_context_driver_vulkan_win32.h"

RenderingContextDriverVulkanWin32::RenderingContextDriverVulkanWin32(GLFWwindow *window)
{
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(get_instance(), window, allocation_callbacks, &surface);
    _initialize_window(surface);
}

RenderingContextDriverVulkanWin32::~RenderingContextDriverVulkanWin32()
{
    /* do nothing in here... */
}