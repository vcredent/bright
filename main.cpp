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
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
};

const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f,  0.5f,  0.0f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f,  0.0f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
};

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

    const char *vertex = "../shader/vertex.glsl.spv";
    const char *fragment = "../shader/fragment.glsl.spv";

    VkVertexInputAttributeDescription attribute[] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) },
            { 0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
            { 0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) },
    };

    RenderingDeviceDriverVulkan::Buffer *vertex_buffer;
    size_t vertices_buf_size = std::size(vertices) * sizeof(Vertex);
    vertex_buffer = rd->create_buffer(vertices_buf_size);
    rd->write_buffer(vertex_buffer, 0, vertices_buf_size, (void *) std::data(vertices));

    RenderingDeviceDriverVulkan::Buffer *index_buffer;
    size_t index_buffer_size = std::size(indices) * sizeof(uint32_t);
    index_buffer = rd->create_buffer(index_buffer_size);
    rd->write_buffer(index_buffer, 0, index_buffer_size, (void *) std::data(indices));

    VkPipeline pipeline = VK_NULL_HANDLE;
    rd->create_graph_pipeline(vertex, fragment,
                              0, nullptr,
                              ARRAY_SIZE(attribute), attribute,
                              &pipeline);

    VkCommandBuffer graph_command_buffer;
    while (!glfwWindowShouldClose(window)) {
        rd->begin_graph_command_buffer(&graph_command_buffer);
        {
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(graph_command_buffer, 0, 1, &vertex_buffer->vk_buffer, &offset);
            vkCmdBindIndexBuffer(graph_command_buffer, index_buffer->vk_buffer, 0, VK_INDEX_TYPE_UINT32);
            rd->command_bind_graph_pipeline(graph_command_buffer, pipeline);
            vkCmdDrawIndexed(graph_command_buffer, std::size(indices), 1, 0, 0, 0);
        }
        rd->end_graph_command_buffer(graph_command_buffer);
        glfwPollEvents();
    }

    rd->destroy_buffer(index_buffer);
    rd->destroy_buffer(vertex_buffer);
    render_context->destroy_render_device(rd);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}