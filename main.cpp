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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <chrono>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
};

struct MVPMatrix {
    glm::mat4 m;
    glm::mat4 v;
    glm::mat4 p;
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

    auto rc = std::make_unique<RenderingContextDriverVulkanWin32>(window);
    // initialize
    rc->initialize();

    RenderingDeviceDriverVulkan *rd;
    rd = rc->load_render_device();

    glfwSetWindowUserPointer(window, rc.get());
    glfwSetWindowSizeCallback(window, [] (GLFWwindow *window, int w, int h) {
        clock_t start, end;

        start = clock();
        auto *driver = (RenderingContextDriverVulkanWin32 *) glfwGetWindowUserPointer(window);
        driver->update_window();
        end = clock();

        printf("glfw resize event exec time: %ldms\n", end - start);
    });

    VkVertexInputBindingDescription binds[] = {
            { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX  }
    };

    VkVertexInputAttributeDescription attributes[] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
            { 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) },
    };

    VkDescriptorSetLayoutBinding descriptor_layout_binds[] = {
            { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE },
    };

    RenderingDeviceDriverVulkan::Buffer *mvp_matrix_buffer;
    mvp_matrix_buffer = rd->create_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(MVPMatrix));

    VkDescriptorSetLayout descriptor_layout;
    rd->create_descriptor_set_layout(ARRAY_SIZE(descriptor_layout_binds), descriptor_layout_binds, &descriptor_layout);

    VkDescriptorSet mvp_descriptor;
    rd->allocate_descriptor(descriptor_layout, &mvp_descriptor);

    RenderingDeviceDriverVulkan::ShaderInfo shader_info = {
            /* vertex= */ "../shader/vertex.glsl.spv",
            /* fragment= */ "../shader/fragment.glsl.spv",
            /* attribute_count= */ ARRAY_SIZE(attributes),
            /* attributes= */ attributes,
            /* bind_count= */ ARRAY_SIZE(binds),
            /* binds= */ binds,
            /* descriptor_count= */ 1,
            /* descriptor_layouts= */ &descriptor_layout,
    };

    RenderingDeviceDriverVulkan::Pipeline *pipeline;
    pipeline = rd->create_graph_pipeline(&shader_info);

    RenderingDeviceDriverVulkan::Buffer *vertex_buffer;
    size_t vertices_buf_size = std::size(vertices) * sizeof(Vertex);
    vertex_buffer = rd->create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, vertices_buf_size);
    rd->write_buffer(vertex_buffer, 0, vertices_buf_size, (void *) std::data(vertices));

    RenderingDeviceDriverVulkan::Buffer *index_buffer;
    size_t index_buffer_size = std::size(indices) * sizeof(uint32_t);
    index_buffer = rd->create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer_size);
    rd->write_buffer(index_buffer, 0, index_buffer_size, (void *) std::data(indices));

    uint32_t index = 0;
    VkRenderPass render_pass;
    VkFramebuffer framebuffer;
    VkSwapchainKHR swap_chain;
    VkCommandBuffer graph_command_buffer;

    VkQueue graph_queue;
    VkSemaphore image_available_semaphore, render_finished_semaphore;
    rc->get_window_semaphore(&image_available_semaphore, &render_finished_semaphore);
    rc->get_graph_queue(&graph_queue);
    rc->get_swap_chain(&swap_chain);

    float speed = 1.0f;

    while (!glfwWindowShouldClose(window)) {
        rc->acquire_next_image(image_available_semaphore, &index);
        rc->acquire_next_framebuffer(&graph_command_buffer, index, &render_pass, &framebuffer);
        rd->command_buffer_begin(&graph_command_buffer);
        {
            VkRect2D rect = {};
            rect.extent = { rc->get_width(), rc->get_height() };
            rd->command_begin_render_pass(graph_command_buffer, render_pass, framebuffer, &rect);
            {
                static auto startTime = std::chrono::high_resolution_clock::now();
                auto currentTime = std::chrono::high_resolution_clock::now();
                float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
                MVPMatrix mvp = {};
                mvp.m = glm::rotate(glm::mat4(1.0f), time * glm::radians(speed), glm::vec3(1.0f, 0.5f, 2.0f));
                mvp.v = glm::lookAt(glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mvp.p = glm::perspective(glm::radians(45.0f), rc->get_width() / (float) rc->get_height(), 0.1f, 10.0f);
                mvp.p[1][1] *= -1;
                rd->write_buffer(mvp_matrix_buffer, 0, sizeof(MVPMatrix), &mvp);

                rd->command_bind_graph_pipeline(graph_command_buffer, pipeline);
                rd->command_bind_descriptor(graph_command_buffer, pipeline, mvp_descriptor);
                rd->write_descriptor_set(mvp_matrix_buffer, mvp_descriptor);

                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(graph_command_buffer, 0, 1, &vertex_buffer->vk_buffer, &offset);
                vkCmdBindIndexBuffer(graph_command_buffer, index_buffer->vk_buffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(graph_command_buffer, std::size(indices), 1, 0, 0, 0);
            }
            rd->command_end_render_pass(graph_command_buffer);
        }
        rd->command_buffer_end(graph_command_buffer);
        VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        rd->command_buffer_submit(graph_command_buffer, 1, &image_available_semaphore, 1, &render_finished_semaphore, &mask, graph_queue, VK_NULL_HANDLE);
        rd->present(graph_queue, swap_chain, index, render_finished_semaphore);
        glfwPollEvents();
    }

    rd->destroy_buffer(mvp_matrix_buffer);
    rd->destroy_buffer(index_buffer);
    rd->destroy_buffer(vertex_buffer);
    rd->destroy_pipeline(pipeline);
    rd->free_descriptor(mvp_descriptor);
    rd->destroy_descriptor_set_layout(descriptor_layout);
    rc->destroy_render_device(rd);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}