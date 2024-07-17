/* ======================================================================== */
/* main.cpp                                                                 */
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
#include "platform/win32/rendering_context_driver_vulkan_win32.h"
#include <memory>
#include <time.h>

int main(int argc, char **argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow *window = glfwCreateWindow(800, 600, "CopilotEngine", nullptr, nullptr);
    assert(window != nullptr);

    auto render_context = std::make_unique<RenderingContextDriverVulkanWin32>(window);
    // initialize
    render_context->initialize();

    RenderingDeviceDriverVulkan *rd;
    rd = render_context->load_render_device();

    glfwSetWindowUserPointer(window, render_context.get());
    glfwSetWindowSizeCallback(window, [] (GLFWwindow *window, int w, int h) {
        clock_t start, end;

        start = clock();
        auto *driver = (RenderingContextDriverVulkanWin32 *) glfwGetWindowUserPointer(window);
        driver->update_window();
        end = clock();

        printf("glfw resize event exec time: %ldms\n", end - start);
    });

    VkPipeline pipeline = VK_NULL_HANDLE;
    rd->create_graph_pipeline(&pipeline);
    vkDestroyPipeline(render_context->get_device(), pipeline, allocation_callbacks);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    render_context->destroy_render_device(rd);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}