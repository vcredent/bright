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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#include "platform/win32/render_device_context_win32.h"
#include <vector>
#include <chrono>
#include "render/camera/track_ball_camera_controller.h"
#include "render/camera/projection_camera.h"
#include "render/renderer_imgui.h"
#include "render/renderer_canvas.h"
#include "render/renderer_screen.h"
#include "render/gui/renderer_viewport.h"

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

    RendererImGui *imgui = memnew(RendererImGui, rd);
    imgui->initialize(screen);

    ProjectionCamera *camera = memnew(ProjectionCamera);
    CameraController *track_ball_controller = memnew(TrackBallCameraController, camera);

    RendererViewport *viewport = memnew(RendererViewport, "视口", imgui);
    viewport->add_window_user_pointer("#CANVAS", canvas);
    viewport->add_window_user_pointer("#TRACK_BALL_CONTROLLER", track_ball_controller);

    viewport->set_window_resize_callback([](RegisterEventCallback *event, int w, int h) {
        event->pointer<RendererCanvas>("#CANVAS")->set_canvas_extent(w, h);
        event->pointer<CameraController>("#TRACK_BALL_CONTROLLER")->get_current_camera()->set_aspect_ratio((float) w / (float) h);
    });

    viewport->set_window_mouse_button_callback([](RegisterEventCallback *event, int button, int action, int mods) {
        CameraController *controller = event->pointer<CameraController>("#TRACK_BALL_CONTROLLER");

        controller->on_event_mouse_button(button, action, mods);

        if (button == INP_MOUSE_BUTTON_LEFT && !action) {
            controller->uncontinued();
            event->get_renderer()->cmd_show_cursor();
        }

        if (button == INP_MOUSE_BUTTON_LEFT && action) {
            controller->on_update_camera();
            event->get_renderer()->cmd_hide_cursor();
        }
    });

    viewport->set_window_cursor_position_callback([](RegisterEventCallback *event, float x, float y) {
        event->pointer<CameraController>("#TRACK_BALL_CONTROLLER")->on_event_cursor(x, y);
    });

    static bool show_demo_flag = true;

    while (window->is_close()) {
        /* poll events */
        window->poll_events();

        /* render to canvas */
        VkCommandBuffer canvas_cmd_buffer;
        canvas->cmd_begin_canvas_render(&canvas_cmd_buffer);
        {
            MVPMatrix mvp;
            mvp.m = glm::translate(Matrix4(1.0f), Vector3(0.0f, 0.0f, 0.0f));
            mvp.v = camera->look_view();
            mvp.p = camera->perspective();
            rd->write_buffer(mvp_matrix_buffer, 0, sizeof(MVPMatrix), &mvp);

            rd->cmd_bind_graph_pipeline(canvas_cmd_buffer, pipeline);
            rd->cmd_bind_descriptor_set(canvas_cmd_buffer, pipeline, mvp_descriptor);
            rd->write_descriptor_set_buffer(mvp_matrix_buffer, mvp_descriptor);
            rd->cmd_setval_viewport(canvas_cmd_buffer, canvas->get_canvas_width(), canvas->get_canvas_height());

            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(canvas_cmd_buffer, 0, 1, &vertex_buffer->vk_buffer, &offset);
            vkCmdBindIndexBuffer(canvas_cmd_buffer, index_buffer->vk_buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(canvas_cmd_buffer, std::size(indices), 1, 0, 0, 0);
        }
        RenderDevice::Texture2D *canvas_texture = canvas->cmd_end_canvas_render();

        /* render to window */
        VkCommandBuffer window_cmd_buffer = screen->cmd_begin_screen_render();
        {
            /* ImGui */
            imgui->cmd_begin_imgui_render(window_cmd_buffer);
            {
                ImGui::ShowDemoWindow(&show_demo_flag);
                viewport->cmd_begin_viewport_render();
                {
                    viewport->cmd_draw_image(canvas_texture);
                }
                viewport->cmd_end_viewport_render();

                imgui->cmd_begin_window("摄像机参数");
                {
                    glm::vec3 position = camera->get_position();
                    imgui->cmd_drag_float3("位置: ", glm::value_ptr(position), 0.01f);
                    camera->set_position(position);

                    float fov = camera->get_fov();
                    imgui->cmd_drag_float("景深: ", &fov, 0.01f);
                    camera->set_fov(fov);

                    float near = camera->get_near();
                    imgui->cmd_drag_float("近点: ", &near, 0.01f);
                    camera->set_near(near);

                    float far = camera->get_far();
                    imgui->cmd_drag_float("远点: ", &far, 0.01f);
                    camera->set_far(far);
                }
                imgui->cmd_end_window();
            }
            imgui->cmd_end_imgui_render(window_cmd_buffer);
        }
        screen->cmd_end_screen_render(window_cmd_buffer);
    }

    memdel(viewport);
    memdel(track_ball_controller);
    memdel(camera);
    memdel(screen);
    memdel(canvas);
    memdel(imgui);
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