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
#include "platform/win32/render_device_context_win32.h"
#include <vector>
#include <chrono>
#include "render/camera/track_ball_camera_controller.h"
#include "render/camera/perspective_camera.h"
#include "render/renderer_editor.h"
#include "render/renderer_canvas.h"
#include "render/renderer_screen.h"

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

static TrackBallCameraController controller;

int main(int argc, char **argv)
{
    Window *window = memnew(Window, "CopilotEngine", 1980, 1080);

    auto rdc = std::make_unique<RenderDeviceContextWin32>(window);
    // initialize
    rdc->initialize();

    RenderDevice *rd;
    rd = rdc->load_render_device();

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

    RenderDevice::Buffer *mvp_matrix_buffer;
    mvp_matrix_buffer = rd->create_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(MVPMatrix));

    VkDescriptorSetLayout descriptor_layout;
    rd->create_descriptor_set_layout(ARRAY_SIZE(descriptor_layout_binds), descriptor_layout_binds, &descriptor_layout);

    VkDescriptorSet mvp_descriptor;
    rd->allocate_descriptor_set(descriptor_layout, &mvp_descriptor);

    RenderDevice::Buffer *vertex_buffer;
    size_t vertices_buf_size = std::size(vertices) * sizeof(Vertex);
    vertex_buffer = rd->create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, vertices_buf_size);
    rd->write_buffer(vertex_buffer, 0, vertices_buf_size, (void *) std::data(vertices));

    RenderDevice::Buffer *index_buffer;
    size_t index_buffer_size = std::size(indices) * sizeof(uint32_t);
    index_buffer = rd->create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer_size);
    rd->write_buffer(index_buffer, 0, index_buffer_size, (void *) std::data(indices));

    PerspectiveCamera camera(45.0f, 0.0f, 0.01, 45.0f);
    controller.make_current_camera(&camera);

    RendererScreen *screen = memnew(RendererScreen, rd);
    screen->initialize(window);

    RenderDevice::ShaderInfo shader_info = {
            /* vertex= */ "../shader/vertex.glsl.spv",
            /* fragment= */ "../shader/fragment.glsl.spv",
            /* attribute_count= */ ARRAY_SIZE(attributes),
            /* attributes= */ attributes,
            /* bind_count= */ ARRAY_SIZE(binds),
            /* binds= */ binds,
            /* descriptor_count= */ 1,
            /* descriptor_layouts= */ &descriptor_layout,
    };

    RenderDevice::Pipeline *pipeline;
    pipeline = rd->create_graph_pipeline(screen->get_render_pass(), &shader_info);

    RendererCanvas *canvas = memnew(RendererCanvas, rd);
    canvas->initialize();

    RendererEditor *editor = memnew(RendererEditor, rd);
    editor->initialize(screen);

    uint32_t viewport_width = 32;
    uint32_t viewport_height = 32;
    static bool show_demo_flag = true;

    while (window->is_close()) {
        /* poll events */
        window->poll_events();

        /* render to canvas */
        VkCommandBuffer canvas_cmd_buffer;
        canvas->cmd_begin_canvas_render(&canvas_cmd_buffer, viewport_width, viewport_height);
        {
            static auto startTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
            MVPMatrix mvp = {};
            mvp.m = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(1.0f, 0.5f, 2.0f));
            mvp.v = camera.look_at();
            camera.set_aspect_ratio((float) viewport_width / (float) viewport_height);
            mvp.p = camera.perspective();
            mvp.p[1][1] *= -1;
            rd->write_buffer(mvp_matrix_buffer, 0, sizeof(MVPMatrix), &mvp);

            rd->cmd_bind_graph_pipeline(canvas_cmd_buffer, pipeline);
            rd->cmd_bind_descriptor_set(canvas_cmd_buffer, pipeline, mvp_descriptor);
            rd->write_descriptor_set_buffer(mvp_matrix_buffer, mvp_descriptor);
            rd->cmd_setval_viewport(canvas_cmd_buffer, viewport_width, viewport_height);

            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(canvas_cmd_buffer, 0, 1, &vertex_buffer->vk_buffer, &offset);
            vkCmdBindIndexBuffer(canvas_cmd_buffer, index_buffer->vk_buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(canvas_cmd_buffer, std::size(indices), 1, 0, 0, 0);
        }
        RenderDevice::Texture2D *canvas_texture = canvas->cmd_end_canvas_render();

        /* render to window */
        controller.on_update();
        ImTextureID im_texture;
        VkCommandBuffer window_cmd_buffer = screen->cmd_begin_window_render();
        {
            /* ImGui */
            editor->cmd_begin_editor_render(window_cmd_buffer);
            {
                ImGui::ShowDemoWindow(&show_demo_flag);
                editor->cmd_begin_viewport("视口");
                {
                    im_texture = editor->create_texture(canvas_texture);
                    editor->cmd_draw_texture(im_texture, &viewport_width, &viewport_height);
                }
                editor->cmd_end_viewport();

                editor->cmd_begin_window("摄像机参数");
                {
                    float fov = camera.get_fov();
                    editor->cmd_drag_float("景深: ", &fov, 0.01f);
                    camera.set_fov(fov);

                    float near = camera.get_near();
                    editor->cmd_drag_float("近点: ", &near, 0.01f);
                    camera.set_near(near);

                    float far = camera.get_far();
                    editor->cmd_drag_float("远点: ", &far, 0.01f);
                    camera.set_far(far);
                }
                editor->cmd_end_window();
            }
            editor->cmd_end_editor_render(window_cmd_buffer);
        }
        screen->cmd_end_window_render(window_cmd_buffer);
        editor->destroy_texture(im_texture);
    }

    memdel(screen);
    memdel(canvas);
    memdel(editor);
    rd->destroy_buffer(mvp_matrix_buffer);
    rd->destroy_buffer(index_buffer);
    rd->destroy_buffer(vertex_buffer);
    rd->destroy_pipeline(pipeline);
    rd->free_descriptor_set(mvp_descriptor);
    rd->destroy_descriptor_set_layout(descriptor_layout);
    rdc->destroy_render_device(rd);
    memdel(window);

    return 0;
}